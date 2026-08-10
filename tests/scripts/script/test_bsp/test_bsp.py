#!/usr/bin/python3
# encoding: utf-8
import pytest
import random
import string
from utils.common import getLog
from utils.report.getspeed import *


# public function
def get_largest_free_memblock(p):
    p.sendCommand('setlogmask i')
    ret = p.sendCommand("memdump free", "Total Blks")
    assert ret == 0
    memdump_free_info = getLog(p.log, "memdump free", "Total Size")
    # print(memdump_free_info)
    (size, addr) = memdump_free_info[-2].split()[-2:]
    test_size = int(size) // 8 * 2
    test_addr = hex(int(addr, 16)+int(hex(test_size), 16))
    p.sendCommand('setlogmask r')
    return (test_size, test_addr)

def get_free_size(p):
    start_token = "".join(random.choice(string.ascii_letters + string.digits) for _ in range(random.randint(5, 20)))
    free_start = f"echo start query:" + start_token

    end_token = "".join(random.choice(string.ascii_letters + string.digits) for _ in range(random.randint(5, 20)))
    free_end = f'echo end query:' + end_token
    print(free_start)
    print(free_end)
    p.sendCommand(free_start)
    ret = p.sendCommand("free", "Umem")
    while ret != 0:
        ret = p.sendCommand("free", "Umem")
    p.sendCommand(free_end)
    

    free_info = getLog(p.log, start_token, end_token)
    result = [Umem.strip() for Umem in free_info if "Umem" in Umem]
    free = result[0].split()[-4]
    test_size = int (free)//2048
    return test_size


