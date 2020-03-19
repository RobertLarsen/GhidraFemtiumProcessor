# Find and mark call, jump and return instructions in the Femtium architecture
# This instruction is actually a return instruction
#
#
#  0040a848   add pc, ra, zz, 0x0
#
#
# This sequence of instructions is a call:
#
#  004199dc   add  ra, pc, zz, 0xc
#  004199e0   movi at0, 0x133f << 0x3
#  004199e4   addi at0, 0x7fff << 0x11
#  004199e8   add  pc, pc, at0, 0x0
#
#  This is a jump:
#
#  0040a640   movi t0, 0x109 << 0x7
#  0040a644   addi t0, 0x1 << 0x16
#  0040a648   add  pc, zz, t0, 0x0
#
#  Jumps always use t0 and calls use at0
#
#
#@author  Robert Larsen
#@category Testing
#@keybinding 
#@menupath 
#@toolbar 

from ghidra.program.model.lang import Register
from ghidra.program.model.listing import CodeUnit

def deductRegisterValue(ins, reg, depth=0, maxDepth=10):
    mnemonic = ins.getMnemonicString() if ins else None
    if depth > maxDepth:
        res = -1
    elif ins is None:
        res = -1
    elif reg == 'zz':
        res = 0
    elif reg == 'pc':
        res = ins.getNext().getAddress().getOffset()
    elif mnemonic == 'add' and ins.getRegister(0).getName() == reg:
        res = deductRegisterValue(ins, ins.getRegister(1).getName(), depth+1, maxDepth) + \
              deductRegisterValue(ins, ins.getRegister(2).getName(), depth+1, maxDepth) + \
              ins.getScalar(3).getValue()
    elif mnemonic == 'addi' and ins.getRegister(0).getName() == reg:
        res = deductRegisterValue(ins.getPrevious(), reg, depth+1, maxDepth) + \
              (ins.getScalar(1).getValue() << ins.getScalar(2).getValue())
    elif mnemonic == 'movi' and ins.getRegister(0).getName() == reg:
        res = ins.getScalar(1).getValue() << ins.getScalar(2).getValue()
    else:
        res = deductRegisterValue(ins.getPrevious(), reg, depth+1, maxDepth)
    return res


call_count = 0
fm = currentProgram.getFunctionManager()
listing = currentProgram.getListing()
for f in fm.getFunctions(True):
    body = f.getBody()
    for i in [x for x in listing.getInstructions(body, True)]:
        # Are we jumping around?
        if i.getMnemonicString() == 'add' and \
           i.getRegister(0).getName() == 'pc' and \
           i.getRegister(1).getName() == 'ra' and \
           i.getRegister(2).getName() == 'zz' and \
           i.getScalar(3).getValue() == 0:
            i.setComment(CodeUnit.EOL_COMMENT, 'Return')
        elif i.getMnemonicString() == 'add' and \
             i.getRegister(0).getName() == 'pc':
            callAddr = (deductRegisterValue(i, i.getRegister(1).getName()) + \
                       deductRegisterValue(i, i.getRegister(2).getName()) + \
                       i.getScalar(3).getValue()) & 0xffffffff
            returnAddress = deductRegisterValue(i, 'ra') & 0xffffffff
            if returnAddress == i.getNext().getAddress().getOffset():
                i.setComment(CodeUnit.EOL_COMMENT, "Call 0x{:x}, return to 0x{:x}".format(callAddr, returnAddress))
                call_count += 1
            else:
                i.setComment(CodeUnit.EOL_COMMENT, 'Jump to 0x{:x}'.format(callAddr))
                print('Jump at {}'.format(i.getAddress()))

print('Found {} calls'.format(call_count))
