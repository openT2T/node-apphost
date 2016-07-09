#include "tests/commons/common-posix.h"

int main(int argc, char **args) {
  JS_DefineMainFile("console.log('>>>', process.execPath)");
  JS_StartEngine("/"); // defaults to main.js

  while (JS_LoopOnce() != 0) usleep(1);

  JS_Value process;
  JS_GetProcessObject(process);

  JS_Value obj;
  JS_CreateEmptyObject(obj);
  JS_SetNamedProperty(obj, "process", process);

  JS_Value val;
  JS_GetNamedProperty(obj, "process", val);

  assert(!JS_IsUndefined(val));
  JS_Free(obj);
  JS_StopEngine();
}
