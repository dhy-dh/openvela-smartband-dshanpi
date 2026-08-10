from enum import Enum


class StatusCodeEnum(Enum):

    NORMAL = (0, "Passed", 0)
    TIMEOUT = (-1, "Timeout", 10)
    EOF = (-2, "EOF", 30)
    CRASH = (-3, "Crash", 25)
    BUSYLOOP = (-4, "Busy loop", 20)
    COMMAND_NOT_FOUND = (-5, "Command not found", 15)
    BLOCK = (-6, "Block", 5)
    UNKNOWN = (-10, "Unknown", 1)

    @staticmethod
    def get_enum_msg_by_code(status_code):
        for status in StatusCodeEnum:
            if status.value[0] == status_code:
                return status.value[1]
        return None

    @staticmethod
    def get_priority_by_code(status_code):
        for status in StatusCodeEnum:
            if status.value[0] == status_code:
                return status.value[2]
        return None

    @staticmethod
    def get_code_by_priority(status_priority):
        for status in StatusCodeEnum:
            if status.value[2] == status_priority:
                return status.value[0]
        return None


class FocusTypeEnum(Enum):

    NONE = (0, "")
    TEST = (1, "Test")
    DEV = (2, "Dev")

    @staticmethod
    def get_enum_msg_by_code(code):
        for data in FocusTypeEnum:
            if data.value[0] == code:
                return data.value[1]
        return None
