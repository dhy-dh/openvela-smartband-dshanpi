#!/usr/bin/python3
# encoding: utf-8
import time
import pytest
from utils.common import getLog
from utils.report.getspeed import *
from utils.common import connectNuttx

from script.test_bsp.test_bsp import get_largest_free_memblock,get_free_size
from utils.common import findString
# from script.test_os.test_os import test_mm


class Test_Mm:

    def common_method(self, p, exec_media_cmd=True):
        p.sendControlCmd("c")
        if exec_media_cmd:
            ret2 = p.sendCommand(
                "media_loop_play -s Capture -c 200 -h 5 -t 1 -u /data/2.wav &", "media_loop_play", timeout=50
            )
            assert ret2 == 0
            p.switch_to_original_core("cp")
            ret4 = p.sendCommand("bttool", "bttool>", timeout=2)
            assert ret4 == 0

            ret10 = p.sendCommand("help", "inquiry", timeout=2)
            if ret10 == 0:
                ret12 = p.sendCommand("enable", ["bttool>", "State=Off, Event=SYS_TURN_ON", "State=On, Event=SYS_TURN_ON"],
                                    timeout=50)
                assert ret12 >= 0
                time.sleep(2)
                ret5 = p.sendCommand("inquiry start 5", "Discovery state: Started", timeout=10)
                if ret5 != 0:
                    p.sendCommand("No expectations were received")
                time.sleep(10)
                ret11 = p.sendCommand("disable",
                                    ["bttool>", "State=Off, Event=SYS_TURN_OFF", "State=On, Event=SYS_TURN_OFF"],
                                    timeout=10)
                assert ret11 >= 0
            else:
                ret5 = p.sendCommand(
                    "gap discovery 5 &", "discovery", timeout=50
                )
                assert ret5 == 0
                p.sendControlCmd("c")
                time.sleep(2)
                p.switch_to_original_core("cp")
                ret8 = p.sendCommand(
                    "gap stopdiscovery", "bttool>", timeout=50
                )
                assert ret8 == 0
            p.sendControlCmd("c")

    def test_ramtest_ram(self, p):

        for i in range(1, 131):
            p.sendControlCmd("c")

            echo_str = f"echo test_ramtest_ram_{i}"
            p.sendCommand(echo_str)

            self.common_method(p)

            p.sendControlCmd("c")
            parameters = ['w', 'h', 'b']
            for para in parameters:
                # get memdump free info
                (size, addr) = get_largest_free_memblock(p)
                # print(size, addr)
                ret9 = p.sendCommand("ramtest -%s -s %s" % (para, size),
                                     "Address-in-address", timeout=60)
                assert ret9 == 0
            p.sendCommand("ps")
        p.reboot()

    def test_ramtest_mm(self, p):

        for i in range(1, 131):
            p.sendControlCmd("c")

            echo_str = f"echo test_ramtest_mm_{i}"
            p.sendCommand(echo_str)

            self.common_method(p)

            p.sendControlCmd("c")
            ret = p.sendCommand("mm", "TEST COMPLETE", timeout=200)
            assert ret == 0
        p.reboot()

    def test_ramtest_opus(self, p):

        for i in range(1, 131):
            p.sendControlCmd("c")

            echo_str = f"echo test_ramtest_opus_{i}"
            p.sendCommand(echo_str)

            self.common_method(p, exec_media_cmd=False)

            p.sendControlCmd("c")
            if i == 1:
                p.sendCommand(
                    "echo V > /dev/watchdog0"
                )

                ret9 = p.sendCommand(
                    "opus_ramtest -s 40960 &", "libopus", timeout=150
                )
                assert ret9 == 0
        p.reboot()

        l1 = ["PANIC!!!", "Assertion"]
        for string in l1:
            assert findString(string, p.log) == False

    def test_ramtest_memstress(self, p):

        for i in range(1, 131):
            p.sendControlCmd("c")

            echo_str = f"echo test_ramtest_memstress_{i}"
            p.sendCommand(echo_str)

            self.common_method(p)

            p.sendControlCmd("c")
            if i == 1:
                p.sendCommand("setlogmask i")
                free_size = get_free_size(p)
                # free_size // 3 * 2 // 100
                ret9 = p.sendCommand("memstress -m %d -n 100 -t 100 &" % (free_size // 150), "MemoryStress", timeout=100)
                assert ret9 == 0
        p.reboot()

        l1 = ["PANIC!!!", "Assertion"]
        for string in l1:
            assert findString(string, p.log) == False

    def test_ramtest_tester(self, p):

        for i in range(1, 131):
            p.sendControlCmd("c")

            echo_str = f"echo test_ramtest_tester_{i}"
            p.sendCommand(echo_str)

            self.common_method(p)

            p.sendControlCmd("c")
            if i == 1:
                p.sendCommand("echo V > /dev/watchdog0")
                free_size = get_free_size(p)
                ret13 = p.sendCommand("memtester %dK 1"%(free_size), ["Done"], timeout=600)
                if ret13 != 0:
                   ret13 = p.sendCommand("memtester %dK 1"%(free_size), ["Done"], timeout=600)
                   assert ret13 == 0
        p.reboot()
