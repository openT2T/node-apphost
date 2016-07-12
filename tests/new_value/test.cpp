#include "../commons/common-posix.h"

JS_Value fnc;
JS_Value param;

void sampleMethod(JS_Value *results, int argc) {
  assert(JS_IsFunction(results[1]) && JS_IsString(results[0]) &&
         "Function parameters do not match");
  JS_Value &fnc_ = results[1];
  JS_Value &param_ = results[0];
  JS_MakePersistent(fnc_);
  JS_MakePersistent(param_);
  
  // copy memory. because results will be free'd as soon as the callback returns
  fnc = fnc_;
  param = param_;
}

const char *contents =
    "function fnc(x, z){return (x + z);}\n"
    "process.fn = fnc;\n"
    "process.natives.sampleMethod('Hello ', fnc);";

int main(int argc, char **args) {
  JS_DefineMainFile(contents);
  JS_SetProcessNative("sampleMethod", sampleMethod);
  JS_StartEngine("/"); // defaults to main.js

  while (JS_LoopOnce() != 0) usleep(1);

  JS_Value value;
  JS_New(value);
  JS_SetString(value, "World", 5);
 
  JS_Value params[2] = {param, value};
  JS_Value out;
  JS_CallFunction(fnc, params, 2, &out);

  JS_Free(value);

  JS_ClearPersistent(fnc);
  JS_Free(fnc);
  JS_ClearPersistent(param);
  JS_Free(param);

  std::string str;
  ConvertResult(out, str);
  assert(strcmp("Hello World", str.c_str()) == 0 && "Result doesn't match.");
  JS_Free(out);

  JS_StopEngine();
}
