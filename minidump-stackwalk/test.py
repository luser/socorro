#!/usr/bin/env python

import ctypes
import ctypes.util
import os

lib = ctypes.cdll.LoadLibrary(os.path.join(os.path.dirname(__file__), "libstackwalk.so"))
paths = ["/tmp/symbols"]
arr = (ctypes.c_char_p * len(paths))(*paths)
dump = '/home/vagrant/src/socorro/google-breakpad/src/processor/testdata/minidump2.dmp'
out_json = ctypes.c_char_p()
out_pipe = ctypes.c_char_p()
lib.process_minidump(dump, arr, len(paths), None, False, True, ctypes.addressof(out_json), ctypes.addressof(out_pipe))
print "%s\n====PIPE DUMP ENDS===\n%s\n" % (out_pipe.value, out_json.value)
libc = ctypes.cdll.LoadLibrary(ctypes.util.find_library("c"))
libc.free(out_json)
libc.free(out_pipe)
