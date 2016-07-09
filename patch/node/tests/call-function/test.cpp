#include "tests/commons/common-posix.h"

void sampleMethod(JS_Value *params, int argc) {
  printf("Called\n");
  JS_Value obj;

  // return
  JS_SetDouble(params[argc], (double)1.2);
}

int main(int argc, char **args) {
  JS_DefineMainFile("console.log('>>>', process.execPath)");
  JS_StartEngine("/"); // defaults to main.js

  JS_SetProcessNative("sampleMethod", sampleMethod);

  while (JS_LoopOnce() != 0) usleep(1);

  JS_Value process;
  JS_GetProcessObject(process);

  JS_Value natives;
  JS_GetNamedProperty(process, "natives", natives);

  JS_Value smethod;
  JS_GetNamedProperty(natives, "sampleMethod", smethod);

  JS_Value out;
  JS_CallFunction(smethod, nullptr, 0, &out);

  assert(JS_IsDouble(out));
  JS_Free(out);

  JS_StopEngine();
}
