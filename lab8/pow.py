#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import base64
import hashlib
import time
from pwn import *

# # r = remote('localhost', 10011);
r = remote('inp111.zoolab.org', 10008);
# r = remote('localhost', 10000);
 
def solve_pow(prefix ):
    prefix = r.recvline().decode().split("'")[1];
    print (prefix)
    print(time.time(), "solving pow ...");
    solved = b''
    for i in range(1000000000):
        h = hashlib.sha1((prefix + str(i)).encode()).hexdigest();
        if h[:6] == '000000':
            solved = str(i).encode();
            print("solved =", solved);
            break;
    print(time.time(), "done.");
    print((b'string S: ', base64.b64encode(solved)))
    r.sendlineafter(b'string S: ', base64.b64encode(solved));
prefix =''
solve_pow(prefix );
r.interactive();

r.close();

# vim: set tabstop=4 expandtab shiftwidth=4 softtabstop=4 number cindent fileencoding=utf-8 :
