// Copyright (c) 2014 The Mozilla Foundation.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of The Mozilla Foundation nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// stackwalker produces a JSON-formatted representation of the
// contents of a minidump, including a stack trace per-thread.

#include <vector>

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

using std::vector;

extern "C" bool process_minidump(const char* minidump_path,
                                 const char** symbol_path_array, int num_symbol_paths,
                                 const char* json_path,
                                 bool pretty, bool pipe,
                                 char** out_json, char** out_pipe);
namespace {
void usage() {
  fprintf(stderr, "Usage: stackwalker [options] <minidump> [<symbol paths]\n");
  fprintf(stderr, "Options:\n");
  fprintf(stderr, "\t--pretty\tPretty-print JSON output.\n");
  fprintf(stderr, "\t--pipe-dump\tProduce pipe-delimited output in addition to JSON output\n");
  fprintf(stderr, "\t--raw-json\tAn input file with the raw annotations as JSON\n");
  fprintf(stderr, "\t--help\tDisplay this help text.\n");
}

} // namespace
int main(int argc, char** argv)
{
  bool pretty = false;
  bool pipe = false;
  char* json_path = nullptr;
  static struct option long_options[] = {
    {"pretty", no_argument, nullptr, 'p'},
    {"pipe-dump", no_argument, nullptr, 'i'},
    {"raw-json", required_argument, nullptr, 'r'},
    {"help", no_argument, nullptr, 'h'},
    {nullptr, 0, nullptr, 0}
  };
  int arg;
  int option_index = 0;
  while((arg = getopt_long(argc, argv, "", long_options, &option_index))
        != -1) {
    switch(arg) {
    case 0:
      if (long_options[option_index].flag != 0)
          break;
      break;
    case 'p':
      pretty = true;
      break;
    case 'i':
      pipe = true;
      break;
    case 'r':
      json_path = optarg;
      break;
    case 'h':
      usage();
      return 0;
    case '?':
      break;
    default:
      fprintf(stderr, "Unknown option: -%c\n", (char)arg);
      usage();
      return 1;
    }
  }

  if (optind >= argc) {
    usage();
    return 1;
  }

  vector<const char*> symbol_paths;
  symbol_paths.insert(symbol_paths.end(), argv + optind + 1, argv + argc);

  char* out_json = nullptr;
  char* out_pipe = nullptr;
  bool result = process_minidump(argv[optind], &symbol_paths[0], symbol_paths.size(),
                                 json_path, pretty, pipe, &out_json, &out_pipe);
  if (pipe && out_pipe) {
    printf("%s", out_pipe);
    printf("====PIPE DUMP ENDS===\n");
    free(out_pipe);
  }
  if (out_json) {
    printf("%s", out_json);
    free(out_json);
  }
  return 0;
}
