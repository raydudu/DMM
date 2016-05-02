#!/usr/bin/python

from __future__ import print_function
import sys
import json

myself=""

class Alloc:
    def __init__(self, count, amount, size):
        self.count = []
        self.size = []
        self.append(count, amount, size)

    def append(self, count, amount, size):
        #TODO: check fo count bigger than len(alloc_*)
        self.pad(count)
        self.count.append(amount)
        self.size.append(size)

    def pad(self, count):
        if (len(self.count) < count):
            pad = [0] * (count - len(self.count));
            self.count.extend(pad)
            self.size.extend(pad)

    def dump(self, bt):
        print("Allocations count:", self.count)
        print("Allocations size:", self.size)
        print("Backtrace:\n%s\n" % (bt))

def is_increasing(arr):
    for i in range(1, len(arr)):
        if (arr[i - 1] > arr[i]):
            return False
    return True

def is_equal(arr):
    for i in range(1, len(arr)):
        if (arr[0] != arr[i]):
            return False
    return True


class Allocs:
    alloc_log = {}
    dumps_count = 0

    def log_alloc(self, bt, amount, size):
        btstr = "\n".join(bt)
        if self.alloc_log.has_key(btstr):
            self.alloc_log[btstr].append(self.dumps_count, amount, size)
        else:
            self.alloc_log[btstr] = Alloc(self.dumps_count, amount, size)

    def calculate_size(self, allocs):
        size = 0
        for i in allocs:
            size = size + i[1]
        return size

    def parse_json(self, fn):
        try:
            f = open(fn, "r")
        except:
            _perr("Failed to open %s" % (fn))
            return

        try:
            j = json.load(f)
        except:
            _perr("Failed to read/serialize json file %s" % (fn))
            return
        finally:
            f.close()

        try:
            for item in j["allocations"]:
                size = self.calculate_size(item["allocations"])
                self.log_alloc(item["backtrace"], item["allocations_amount"], size);
        except Exception as e:
            _perr("Failed to parse json, no expected fields:", e)

        self.dumps_count += 1


    def find_increasing_cnt(self):
        for bt, alloc in self.alloc_log.iteritems():
            alloc.pad(self.dumps_count)
            if is_increasing(alloc.count) and not is_equal(alloc.count):
                alloc.dump(bt)

    def find_increasing_size(self):
        for bt, alloc in self.alloc_log.iteritems():
            alloc.pad(self.dumps_count)
            if is_increasing(alloc.size) and not is_equal(alloc.size):
                alloc.dump(bt)

    def bigger_than(self, size):
        for bt, alloc in self.alloc_log.iteritems():
            alloc.pad(self.dumps_count)
            if alloc.size[self.dumps_count - 1] > size:
                alloc.dump(bt)


    def list_all(self):
        print("Total allocations parsed:", len(self.alloc_log))
        for bt, alloc in self.alloc_log.iteritems():
            alloc.pad(self.dumps_count)
            alloc.dump(bt)


def _perr(*msg):
    print("ERROR:", *msg, file=sys.stderr)

def print_help():
    print("Usage: %s <dmm_*.json...dmm_*.json>" % (myself))

def _main(args):
    if len(args) < 2:
        print_help()
        sys.exit(1)

    adb = Allocs()
    for j in args:
        print("Parsing %s ..." % (j))
        adb.parse_json(j)
        print("Parsing %s Done!" % (j))

    # adb.find_increasing_size()
    # adb.find_increasing_count()
    adb.bigger_than(10000)
    # adb.list_all()

    sys.exit(0)

if __name__ == "__main__":
    myself = sys.argv[0]
    _main(sys.argv[1:])

