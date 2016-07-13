#include "../commons/common-posix.h"

void sampleMethod(JS_Value *results, int argc) {
  assert(argc == 3 && "number of arguments supposed to be 3");

  assert(JS_IsFunction(results[0]) && JS_IsFunction(results[1]) &&
         "both parameters supposed to be a function");

  assert(JS_IsString(results[2]) &&
         "third parameter supposed to be a string");

  JS_Value out;
  assert(JS_CallFunction(results[0], &results[2], 1, &out) &&
         "failed while calling console.log");
  assert(JS_IsUndefined(out) &&
         "return value from console.log should be undefined");
}

const char *contents =
    "var func = console.log;\n"
    "func2 = MyObject.sampleMethod;\n"
    "MyObject.sampleMethod(func, func2, 'text');\n";

void OnInit() {
  JS_Value obj;
  JS_CreateEmptyObject(obj);

  JS_Value global;
  JS_GetGlobalObject(global);

  JS_SetNamedProperty(global, "MyObject", obj);

  JS_SetNativeMethod(obj, "sampleMethod", sampleMethod);
  // Free'ing object here wouldn't destroy it on JS land
  JS_Free(obj);
}

int main(int argc, char **args) {
  JS_DefineMainFile(contents);
  JS_DefineNodeOnInitCallback(OnInit);
  JS_StartEngine("/");

  // loop for possible IO
  // or JS_Loop() without usleep / while
  while (JS_LoopOnce() != 0) usleep(1);

  JS_StopEngine();
}
