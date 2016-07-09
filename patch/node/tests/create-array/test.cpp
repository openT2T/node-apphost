#include "tests/commons/common-posix.h"

void sampleMethod(JS_Value *params, int argc) {
  JS_Value obj;
  assert(argc >= 2);

  JS_CreateArrayObject(obj);
  JS_SetIndexedProperty(obj, 0, params[0]);
  JS_SetIndexedProperty(obj, 1, params[1]);

  // return
  JS_SetObject(params[argc], obj);
  JS_Free(obj);
}

void crashMe(JS_Value *_, int argc) {
  assert(0 && "previous call to sampleMethod must be failed");
}

const char *contents =
    "var arr = process.natives.sampleMethod(1,2);"
    "if (!arr || arr[0] !== 1 || arr[1] !== 2) "
    "  process.natives.crashMe();\n"
    "console.log('PASSED')";

int main(int argc, char **args) {
  JS_DefineMainFile(contents);
  JS_SetProcessNative("crashMe", crashMe);
  JS_SetProcessNative("sampleMethod", sampleMethod);

  JS_StartEngine("/"); // defaults to main.js

  while (JS_LoopOnce() != 0) usleep(1);

  JS_StopEngine();
}
