import cstruct
import struct


class Superblock(cstruct.CStruct):
    __byte_order__ = cstruct.BIG_ENDIAN
    __struct__ = '''
    unsigned int magic;
    unsigned int version;
    unsigned int blocksize;
    unsigned int blockcount;
    unsigned int rootnode;
    '''


class Cred(cstruct.CStruct):
    __byte_order__ = cstruct.BIG_ENDIAN
    __struct__ = '''
    unsigned int uid;
    unsigned int gid;
    '''


class Timespec(cstruct.CStruct):
    __byte_order__ = cstruct.BIG_ENDIAN
    __struct__ = '''
    unsigned int time_sec;
    unsigned int time_nsec;
    '''


class INode(cstruct.CStruct):
    __byte_order__ = cstruct.BIG_ENDIAN
    __struct__ = '''
    unsigned int indirection;
    struct Cred owner;
    struct Timespec atime;
    struct Timespec mtime;
    struct Timespec ctime;
    unsigned short perm;
    unsigned short links;
    unsigned int size;
    '''


class DEntry(cstruct.CStruct):
    __byte_order__ = cstruct.BIG_ENDIAN
    __struct__ = '''
    unsigned int dnode;
    unsigned char dtype;
    '''


DTYPE_DIR = 1
DTYPE_FILE = 2
DTYPE_CDEV = 3
DTYPE_BDEV = 4
DTYPE_LNK = 5
DTYPE_PIPE = 6
DTYPE_SOCKE = 7


class FileSystem(object):
    def __init__(self, path, offset=0x2000):
        self.path = path
        self.offset = offset
        self.data = open(path, 'rb').read()
        self.superblock = Superblock()
        self.superblock.unpack(self.data[offset:offset+len(self.superblock)])
        if self.superblock.magic != 0x55354653:
            raise Exception("Bad magic value")
        if self.superblock.version != 1:
            raise Exception("Bad filesystem version")

    def block(self, idx, replacement=None):
        o = self.offset + self.superblock.blocksize * idx
        if replacement is None:
            return self.data[o:o + self.superblock.blocksize]
        else:
            if len(replacement) != self.superblock.blocksize:
                raise Exception('Replacement block has bad size')
            for i in range(self.superblock.blocksize):
                self.data[o + i] = replacement[i]

    def root_inode(self):
        return self.inode(self.superblock.rootnode, DTYPE_DIR)

    def inode_from_dentry(self, entry):
        return self.inode(entry.dnode, entry.dtype)

    def inode_from_path(self, path):
        parts = path.split('/')[1:]
        cur = self.root_inode()

        if path == '/':
            return cur

        while len(parts):
            part = parts[0]
            parts = parts[1:]
            f = next(filter(lambda e: e[1] == part, cur[2]), None)
            if f is None:
                raise Exception('Path not found')
            cur = self.inode_from_dentry(f[0])
        return cur

    def inode(self, idx, t):
        b = self.block(idx)
        node = INode()
        node.unpack(b[:len(node)])
        b = b[len(node):]
        if t == DTYPE_DIR:
            data = b[:node.size]
            def dentries(d):
                r = []
                i = 0
                while i < len(d):
                    e = DEntry()
                    e.unpack(d[i:i + len(e)])
                    i += len(e)
                    n = b''
                    while d[i] != 0:
                        n += bytes([d[i]])
                        i += 1
                    r.append((e, n.decode('utf-8')))
                    i += 1
                return r
            return (t, node, dentries(data))
        if t == DTYPE_FILE:
            blocks = int((node.size + self.superblock.blocksize - 1) / self.superblock.blocksize)
            return (t, node, [struct.unpack('>I', b[i*4:(i+1)*4])[0] for i in range(blocks)])
        if t == DTYPE_LNK:
            s = struct.unpack('>HH', b[:4])[1]
            return (t, node, b[4:4 + s].decode('utf-8'))
        if t in (DTYPE_CDEV, DTYPE_BDEV):
            e = struct.unpack('>HHH', b[:6])
            return (t, node, (e[1], e[2]))

        return (None, node, None)


def perm(p):
    def pe(p):
        r = ''
        r += 'r' if p & (1 << 2) else '-'
        r += 'w' if p & (1 << 1) else '-'
        r += 'x' if p & (1 << 0) else '-'
    return pe((p >> 6) & 0o7) + pe((p >> 3) & 0o7) + pe((p >> 0) & 0o7)


if __name__ == '__main__':
    import sys
    fs = FileSystem(sys.argv[1])
    print(fs.inode_from_path('/dev/null'))
