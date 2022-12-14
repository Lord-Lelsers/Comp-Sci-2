from counter import *
from timealarm import *
from clock import *
from os.path import isfile


class AlarmClock(object):
    """a 24-hour clock object, made of three counter objects: one for the seconds
    (0->59), one for the minutes (0->59), and one for the hour (0->23)."""

    def __init__(self, hour, min, sec):
        """construct counter obj, given hours minutes and seconds"""
        self.hourCounter = Counter(24)
        self.hourCounter.setValue(hour)
        self.minCounter = Counter(60)
        self.minCounter.setValue(min)
        self.secCounter = Counter(60)
        self.secCounter.setValue(sec)
        self.alarm = Timealarm(-1, -1, -1)  # just create one

    def __str__(self):
        returnVal = "Clock: %i:%i:%i" % (
            self.hourCounter.value,
            self.minCounter.value,
            self.secCounter.value,
        )
        return returnVal

    def getTime(self):
        spacer1 = ""
        spacer2 = ""
        if self.minCounter.value < 10:
            spacer1 = "0"
        if self.secCounter.value < 10:
            spacer2 = "0"
        returnVal = "%2i:%s%i:%s%i" % (
            self.hourCounter.value,
            spacer1,
            self.minCounter.value,
            spacer2,
            self.secCounter.value,
        )
        return returnVal

    def getHour(self):
        return self.hourCounter.value

    def getMin(self):
        return self.minCounter.value

    def getSec(self):
        return self.secCounter.value

    def setHour(self, newValue):
        self.hourCounter.setValue(newValue)

    def setMin(self, newValue):
        self.minCounter.setValue(newValue)

    def setSec(self, newValue):
        self.secCounter.setValue(newValue)

    def tick(self):
        if self.secCounter.increment():
            if self.minCounter.increment():
                self.hourCounter.increment()
        if self.alarm.inAlarm(Clock(self.getHour(), self.getMin(), self.getSec())):
            return True
        return False

    def setAlarm(self, hour, min, sec):
        self.alarm.hour = hour
        self.alarm.min = min
        self.alarm.sec = sec

    def findAlarmTime(self, fname):
        if isfile(fname):
            infile = open(fname, "r")
            for line in infile:
                nonFormattedString = line.strip()
                self.alarm.load(nonFormattedString)
            infile.close()
        else:
            nonFormattedString = input("set alarm (hours:min:sec): ")
            self.alarm.load(nonFormattedString)
            print("saving alarm: %s" % self.alarm.formatSave())
            outfile = open(fname, "w")
            outfile.write(self.alarm.formatSave())
            outfile.close()


if __name__ == "__main__":
    c1 = Clock(12, 55, 21)
    print(c1)
    print("Setting time to 23:59:55...")
    c1.setHour(23)
    c1.setMin(59)
    c1.setSec(55)
    print("Hour for c1: %d" % (c1.getHour()))
    print(c1)
    for i in range(15):
        c1.tick()
        print(c1)
