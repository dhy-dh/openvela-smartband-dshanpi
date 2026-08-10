#!/usr/bin/python3
import argparse
import json
import os
import re
import shutil
import sys
import time


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("-p", dest="path", default=".", help="specify nuttx path")
    parser.add_argument(
        "-b",
        dest="board",
        default="bes",
        help="specify board, for example: sim, bes,sim_rpserver,sim_rpproxy",
    )
    parser.add_argument(
        "-r", dest="run", help="specify run value, for example: ap_test"
    )
    args = parser.parse_args()
    return args


def readJson(filename, flag="all"):
    l1 = []
    tmp = []
    with open(filename, "r", encoding="utf-8") as f:
        lines = f.read()
    f.close()
    items = json.loads(lines.replace("'", '"'))
    nflag = "-" + flag
    for key in items.keys():
        for i in range(len(items[key])):
            for j in range(len(items[key][i]["profile"])):
                if flag == items[key][i]["profile"][j]:
                    tmp += items[key][i]["config"]
                # replace "+" with "-" if nflag
                elif nflag == items[key][i]["profile"][j]:
                    tmp += [
                        items[key][i]["config"][k].replace("+", "-")
                        for k in range(len(items[key][i]["config"]))
                    ]

    for i in range(len(tmp)):
        #if "-" in tmp[i] and "=" in tmp[i]:
        #    continue
        l1.append(tmp[i])
    return l1


def inRun(run, *argv):
    for item in argv:
        if item in run:
            return True
    return False


def main():
    args = parse_args()
    path = args.path
    run = args.run
    board = args.board

    # add tweakPath to PATH
    tweakPath = os.path.join(path, "prebuilts/kconfig-frontends/bin/")
    sys.path.append(tweakPath)

    # generate .config file
    configFile = os.path.join(path, "nuttx/.config")

    # read component.txt
    comFile = os.path.join(
        os.path.abspath((os.path.dirname(__file__))), "component.json"
    )

    # common test config
    comList = readJson(comFile, flag="all")
    # config for board
    comList += readJson(comFile, flag=board)

    # sim
    if board in ["quickapp"]:
        f1 = "sim"
        comList += readJson(comFile, flag=f1)

    if "without_pm" in run:
        f1 = board + "_" + run.split('_')[0]
    else:
        f1 = board + "_" + '_'.join(run.split('_')[:-1])

    if inRun(run, "monkey", "debug", "sensor", "ota", "open_source"):
        comList = readJson(comFile, flag=f1)
    if "open_source" in run:
        comList += readJson(comFile, flag="all_open_source")
    else:
        comList += readJson(comFile, flag=f1)

    print(comList)

    # enable or disable component by kconfig-tweak
    flag = ""

    os.chdir(os.path.join(path, "nuttx"))
    # add component
    for com in comList:
        if "+" in com:
            flag = "--enable %s" % com.strip("+")
        if "-" in com:
            flag = "--disable %s" % com.strip("-")
        if "=" in com:
            if com.strip("+").split("=")[1].isdigit():
                flag = "--set-val %s %s" % (
                    com.strip("+").split("=")[0],
                    com.strip("+").split("=")[1],
                )
            else:
                flag = "--set-str %s %s" % (
                    com.strip("+").split("=")[0],
                    com.strip("+").split("=")[1],
                )

        if flag:
            cmd = "%s/kconfig-tweak --file %s " % (tweakPath, configFile) + flag
            os.system(cmd)
            print(cmd)


if __name__ == "__main__":
    main()
