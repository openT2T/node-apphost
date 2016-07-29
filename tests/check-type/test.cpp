#ifdef _WIN32
#include <Windows.h>
#include "Header.h"
#define usleep Sleep
#else
#include "../commons/common-posix.h"
#endif

const char compare_base[10] = {'1', '4', 't', 't', '{',
                               'A', 'u', 'n', 'E', 'n'};

void sampleMethod(JS_Value *results, int argc) {
  for (int i = 0; i < argc; i++) {
    std::string str;
    ConvertResult(results[i], str);
    if (compare_base[i] != str.c_str()[0]) {
      flush_console("FAIL! Item(%d) : %s \n", i, str.c_str());
      exit(-1);
    }
  }

  JS_Value out;
  JS_CallFunction(results + 9, (results + 3), 2, &out);

  assert(JS_GetDataLength(&out) == 11 &&
         "Expected return value was 'test{\"a\":3}");
  JS_Free(&out);
  assert(out.data_ == NULL && out.size_ == 0 && "JS_FreeResultData leaks?");
}

const char *contents =
    "function fnc(x, y){return (x + JSON.stringify(y));}\n"
    "var bf = new Buffer(5);bf.fill(65);\n"
    "process.natives.sampleMethod(1, 4.5, true, 'test', {a:3}, bf, undefined, "
    "null, new Error('error!'), fnc);";

int main(int argc, char **args) {
  JS_DefineMainFile(contents);
  JS_SetProcessNative("sampleMethod", sampleMethod);
  JS_StartEngine("/"); // defaults to main.js

  while (JS_LoopOnce() != 0) usleep(1);

  JS_StopEngine();

  return 0;
}
