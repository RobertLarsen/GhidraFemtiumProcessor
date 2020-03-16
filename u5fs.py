#!/usr/bin/env python3

import traceback, apport
from u5 import FileSystem, DTYPE_DIR, DTYPE_FILE, DTYPE_CDEV, DTYPE_BDEV, \
               DTYPE_LNK, DTYPE_PIPE, DTYPE_SOCKE
import sys
import os

S_IFCHR = 0x2000
S_IFBLK = 0x6000


def extract_dir(fs, path, inode, entries):
    for dentry, name in entries:
        child_path = path + name
        child_type, child_inode, child_data = fs.inode_from_dentry(dentry)
        if child_inode.indirection:
            print('{} has indirection'.format(child_path))

        if child_type == DTYPE_DIR:
            os.mkdir(child_path)
        elif child_type in (DTYPE_CDEV, DTYPE_BDEV):
            os.mknod(child_path, 
                     0o600 | (S_IFCHR 
                              if child_type == DTYPE_CDEV
                              else S_IFBLK),
                     os.makedev(child_data[0], child_data[1]))
        elif child_type == DTYPE_FILE:
            to_write = child_inode.size
            with open(child_path, 'wb') as f:
                for block_id in child_data:
                    block = fs.block(block_id)
                    b = len(block) if to_write > len(block) else to_write
                    f.write(block[:b])
                    to_write -= b
        elif child_type == DTYPE_LNK:
            os.symlink(child_data, child_path)

        try:
            os.chmod(child_path, child_inode.perm)
            os.chown(child_path, child_inode.owner.uid, child_inode.owner.gid)
        except:
            pass
        
        if child_type == DTYPE_DIR:
            extract_dir(fs, child_path + '/', child_inode, child_data)


def extract(fs_path, extract_path):
    fs = FileSystem(fs_path)
    if not os.path.exists(extract_path):
        os.mkdir(extract_path)
    os.chroot(extract_path)
    root = fs.root_inode()
    extract_dir(fs, '/', root[1], root[2])


# python u5fs <binary> extract <path>
if len(sys.argv) < 4:
    print('Usage: {} <fs path> <command> [args]'.format(sys.argv[0]))
elif sys.argv[2] == 'extract':
    extract(sys.argv[1], sys.argv[3])
