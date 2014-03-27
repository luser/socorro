#!/usr/bin/env python

import ctypes
import ctypes.util
import os

def read_file(filename):
    if not filename:
        return None
    with open(filename, 'rb') as f:
        return f.read()

lib = ctypes.cdll.LoadLibrary(os.path.join(os.path.dirname(__file__), "libstackwalk.so"))
paths = ["/tmp/symbols"]
arr = (ctypes.c_char_p * len(paths))(*paths)
dump = '/home/vagrant/src/socorro/google-breakpad/src/processor/testdata/minidump2.dmp'
json_file = None
out_json = ctypes.c_char_p()
out_pipe = ctypes.c_char_p()
dump_contents = read_file(dump)
json_contents = read_file(json_file)
lib.process_minidump(dump_contents, len(dump_contents), arr, len(paths), json_contents if json_contents else None, len(json_contents) if json_contents else None, False, True, ctypes.addressof(out_json), ctypes.addressof(out_pipe))
print "%s\n====PIPE DUMP ENDS===\n%s\n" % (out_pipe.value, out_json.value)
libc = ctypes.cdll.LoadLibrary(ctypes.util.find_library("c"))
libc.free(out_json)
libc.free(out_pipe)
