#!/usr/bin/python3
# encoding: utf-8
import json
import os
import time

import uiautomator2 as u2


def read_btproperties(settings):
    with open(
        os.path.dirname(os.path.abspath(__file__)) + "/btsetting.json",
        "r",
        encoding="utf8",
    ) as fp:
        json_data = json.load(fp)
        proper = json_data[settings]
    return proper


class BaseOperate(object):
    def __init__(self, device_name, app_name):
        self.d = u2.connect(device_name)
        self.d.settings["xpath_debug"] = True
        self.d.healthcheck()
        self.d.screen_on()
        time.sleep(1)
        self.d.swipe_ext("up")
        self.d.toast.show("正在启动app")
        self.d.app_start(app_name, stop=True)
        time.sleep(2)
        self.d.app_start(app_name)
        self.d.watcher.when("跳过").click()
        self.d.watcher.start()

    def findElement(self, elem, timeout=10):
        self.ele = None
        elem_pos = read_btproperties(elem)
        if self.ele == None and elem_pos["xpath"] != "none":
            if self.d.xpath(elem_pos["xpath"]).wait(timeout=timeout):
                self.ele = self.d.xpath(elem_pos["xpath"])
        if self.ele == None and elem_pos["id"] != "none":
            if self.d(resourceId=elem_pos["id"]).wait(timeout=timeout):
                self.ele = self.d(resourceId=elem_pos["id"])
        if self.ele == None and elem_pos["text"] != "none":
            if self.d(text=elem_pos["text"]).wait(timeout=timeout):
                self.ele = self.d(text=elem_pos["text"])
        if self.ele == None and elem_pos["textContains"] != "none":
            if self.d(textContains=elem_pos["textContains"]).wait(timeout=timeout):
                self.ele = self.d(textContains=elem_pos["textContains"])
        if self.ele == None and elem_pos["description"] != "none":
            if self.d(description=elem_pos["description"]).wait(timeout=timeout):
                self.ele = self.d(description=elem_pos["description"])
        if self.ele:
            return True
        else:
            return False

    def clickElement(self, elem, timeout=10):
        if self.findElement(elem=elem, timeout=timeout):
            self.ele.click(timeout=timeout)

    def setValue(self, elem, value, timeout=10):
        if self.findElement(elem=elem, timeout=timeout):
            self.ele.clear_text()
            self.ele.set_text(value)

    def getValue(self, elem, timeout=10):
        if self.findElement(elem=elem, timeout=timeout):
            gettxt = self.ele.get_text(timeout=timeout)
            if gettxt:
                return gettxt
            else:
                return False

    def getToast(self, toast):
        toasttxt = self.d.toast.get_message(5.0, 10.0, False)
        if toasttxt:
            return toasttxt
        else:
            return False

    def press(self, pressback):
        self.d.press(pressback)

    def getInfo(self, elem, timeout=10):
        if self.findElement(elem=elem, timeout=timeout):
            return self.ele.info
        else:
            return False

    def findElementBySwipeUp(self, elem, timeout=10):
        ret = 0
        while True:
            page_content = self.d.dump_hierarchy()[
                (len(self.d.dump_hierarchy()) // 2) :
            ]
            if self.findElement(elem=elem, timeout=timeout):
                ret = 1
                break
            self.d.swipe_ext("up")
            new_page_content = self.d.dump_hierarchy()[
                (len(self.d.dump_hierarchy()) // 2) :
            ]
            if new_page_content == page_content:
                break
        if ret == 1:
            return True
        else:
            return False

    def clickElementBySwipeUp(self, elem, timeout=10):
        if self.findElementBySwipeUp(elem=elem, timeout=timeout):
            self.ele.click(timeout=timeout)

    def swipeExt(self, way, scale=0.6):
        self.d.swipe_ext(way, scale=scale)

    def longClick(self, elem, timeout=10):
        if self.findElement(elem=elem, timeout=timeout):
            self.ele.long_click()

    def longClickBySwipe(self, elem, timeout=10):
        if self.findElementBySwipeUp(elem=elem, timeout=timeout):
            self.ele.long_click()

    def openOtherApp(self, app_name):
        self.d.app_start(app_name)
