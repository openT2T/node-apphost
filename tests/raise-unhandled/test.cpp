#ifdef _WIN32
#include <Windows.h>
#include "Header.h"
#define usleep Sleep
#else
#include "../commons/common-posix.h"
#endif

void sampleMethod(JS_Value *params, int argc) {
  // returns JS return value(5) back to the caller
  JS_Evaluate("var z=4;z+1", "test_scope.js", params + argc);
}

void crashMe(JS_Value *_, int argc) {
  assert(0 && "previous call to sampleMethod must be failed");
}

const char *contents = "if (process.natives.sampleMethod() != 5)\n"
                       "  process.natives.crashMe();\n";

int main(int argc, char **args) {
  JS_DefineMainFile(contents);
  JS_SetProcessNative("crashMe", crashMe);
  JS_SetProcessNative("sampleMethod", sampleMethod);
  JS_StartEngine("/");

  while (JS_LoopOnce() != 0) usleep(1);

  JS_StopEngine();
}
