#ifdef _WIN32
#include <Windows.h>
#include "Header.h"
#define usleep Sleep
#else
#include "../commons/common-posix.h"
#endif

char *xx = NULL;
void sampleMethod(JS_Value *results, int argc) {
  assert(argc>0 && "Test failed");

  if (argc == 2) {
    xx = (char*) malloc (2);
    memcpy(xx, "x\0", 2);
    JS_WrapObject(results+0, xx);
  } else {
    JS_Value str;
    JS_New(&str);

    void *ptr = JS_UnwrapObject(results+0);
    char *dummy = (char*) ptr;
    assert(ptr && "This shouldn't be NULL");

    JS_SetString(&str, dummy, strlen(dummy));

    *(results+argc) = str;
  }
  fflush(stdout);
}

const char *contents =
    "var testObject = {};\n"
    "process.natives.sampleMethod(testObject, true);\n"
    "if (process.natives.sampleMethod(testObject) !== 'x')\n"
    "  process.natives.sampleMethod();";

int main(int argc, char **args) {
  JS_DefineMainFile(contents);
  JS_SetProcessNative("sampleMethod", sampleMethod);
  JS_StartEngine("/"); // defaults to main.js

  while (JS_LoopOnce() != 0) usleep(1);


  while (JS_LoopOnce() != 0) usleep(1);

  JS_StopEngine();

  if (xx) free(xx);
}
