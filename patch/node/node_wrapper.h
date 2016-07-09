// See LICENSE file

#ifndef SRC_JS_HEADER_
#define SRC_JS_HEADER_

#include <stdlib.h>
#include <stdint.h>

#ifdef _WIN32
#define NWRAP_EXTERN(x) __declspec(dllexport) x
#else
#define NWRAP_EXTERN(x) x
#endif

#ifdef __cplusplus
extern "C" {
#endif

enum _JSBaseType {
  RT_Int32 = 1,
  RT_Double = 2,
  RT_Boolean = 3,
  RT_String = 4,
  RT_Object = 5,
  RT_Buffer = 6,
  RT_Undefined = 7,
  RT_Null = 8,
  RT_Error = 9,
  RT_Function = 10
};

typedef enum _JSBaseType JS_ValueType;

struct _JSValue {
  // internal use only
  void *com_;
  bool persistent_;

  void *data_;
  size_t size_;
  JS_ValueType type_;
};

typedef struct _JSValue JS_Value;

NWRAP_EXTERN(bool)
JS_CallFunction(JS_Value &fnc, JS_Value *params, const int argc, JS_Value *out);

NWRAP_EXTERN(bool)
JS_MakePersistent(JS_Value &value);

NWRAP_EXTERN(bool)
JS_ClearPersistent(JS_Value &value);

NWRAP_EXTERN(bool)
JS_IsFunction(JS_Value &value);

NWRAP_EXTERN(bool)
JS_IsError(JS_Value &value);

NWRAP_EXTERN(bool)
JS_IsInt32(JS_Value &value);

NWRAP_EXTERN(bool)
JS_IsDouble(JS_Value &value);

NWRAP_EXTERN(bool)
JS_IsBoolean(JS_Value &value);

NWRAP_EXTERN(bool)
JS_IsString(JS_Value &value);

NWRAP_EXTERN(bool)
JS_IsJSON(JS_Value &value);

NWRAP_EXTERN(bool)
JS_IsBuffer(JS_Value &value);

NWRAP_EXTERN(bool)
JS_IsUndefined(JS_Value &value);

NWRAP_EXTERN(bool)
JS_IsNull(JS_Value &value);

NWRAP_EXTERN(bool)
JS_IsNullOrUndefined(JS_Value &value);

NWRAP_EXTERN(bool)
JS_IsObject(JS_Value &value);

NWRAP_EXTERN(int32_t)
JS_GetInt32(JS_Value &value);

NWRAP_EXTERN(double)
JS_GetDouble(JS_Value &value);

NWRAP_EXTERN(bool)
JS_GetBoolean(JS_Value &value);

NWRAP_EXTERN(char *)
JS_GetString(JS_Value &value);

NWRAP_EXTERN(int32_t)
JS_GetDataLength(JS_Value &value);

NWRAP_EXTERN(char *)
JS_GetBuffer(JS_Value &value);

NWRAP_EXTERN(void)
JS_SetInt32(JS_Value &value, const int32_t val);

NWRAP_EXTERN(void)
JS_SetDouble(JS_Value &value, const double val);

NWRAP_EXTERN(void)
JS_SetBoolean(JS_Value &value, const bool val);

#ifdef __cplusplus
NWRAP_EXTERN(void)
JS_SetString(JS_Value &value, const char *val, const int32_t length = 0);
#else
NWRAP_EXTERN(void)
JS_SetString(JS_Value &value, const char *val, const int32_t length);
#endif

#ifdef __cplusplus
NWRAP_EXTERN(void)
JS_SetUCString(JS_Value &value, const uint16_t *val, const int32_t length = 0);
#else
NWRAP_EXTERN(void)
JS_SetUCString(JS_Value &value, const uint16_t *val, const int32_t length);
#endif

#ifdef __cplusplus
NWRAP_EXTERN(void)
JS_SetJSON(JS_Value &value, const char *val, const int32_t length = 0);
#else
NWRAP_EXTERN(void)
JS_SetJSON(JS_Value &value, const char *val, const int32_t length);
#endif

#ifdef __cplusplus
NWRAP_EXTERN(void)
JS_SetError(JS_Value &value, const char *val, const int32_t length = 0);
#else
NWRAP_EXTERN(void)
JS_SetError(JS_Value &value, const char *val, const int32_t length);
#endif

#ifdef __cplusplus
NWRAP_EXTERN(void)
JS_SetBuffer(JS_Value &value, const char *val, const int32_t length = 0);
#else
NWRAP_EXTERN(void)
JS_SetBuffer(JS_Value &value, const char *val, const int32_t length);
#endif

NWRAP_EXTERN(void)
JS_SetUndefined(JS_Value &value);

NWRAP_EXTERN(void)
JS_SetNull(JS_Value &value);

NWRAP_EXTERN(void)
JS_SetObject(JS_Value &host, JS_Value &val);

NWRAP_EXTERN(void)
JS_Free(JS_Value &value);

NWRAP_EXTERN(bool)
JS_New(JS_Value &value);

NWRAP_EXTERN(bool)
JS_CreateEmptyObject(JS_Value &value);

NWRAP_EXTERN(bool)
JS_CreateArrayObject(JS_Value &value);

NWRAP_EXTERN(void)
JS_SetNamedProperty(JS_Value &object, const char *name, JS_Value &prop);

NWRAP_EXTERN(void)
JS_SetIndexedProperty(JS_Value &object, const unsigned index, JS_Value &prop);

NWRAP_EXTERN(void)
JS_GetNamedProperty(JS_Value &object, const char *name, JS_Value &out);

NWRAP_EXTERN(void)
JS_GetIndexedProperty(JS_Value &object, const int index, JS_Value &out);

NWRAP_EXTERN(void)
JS_GetGlobalObject(JS_Value &out);

NWRAP_EXTERN(void)
JS_GetProcessObject(JS_Value &out);

NWRAP_EXTERN(void)
JS_WrapObject(JS_Value &object, void *ptr);

NWRAP_EXTERN(void *)
JS_UnwrapObject(JS_Value &object);

typedef void (*JS_CALLBACK)(JS_Value *result, int argc);

// Evaluates a JavaScript code on the fly.
// Remarks:
// 1 - returns false if compilation fails or an internal issue happens
// (i.e. no memory)
// 2 - result is a return value from the running code.
// i.e. "var x=4; x+1" returns 5
// 3 - script_name represents the script's file name
// 4 - script_code expects a Utf8/ASCII JavaScript code with \0 ending
NWRAP_EXTERN(bool)
JS_Evaluate(const char *script_code, const char *script_name, JS_Value &result);

// Define the contents of main.js file (entry script)
NWRAP_EXTERN(void)
JS_DefineMainFile(const char *data);

// Define a JavaScript file with it's contents,
// native code: JS_DefineFile("test.js", "exports.x=4");
// js code: require('test.js').x -> 4
NWRAP_EXTERN(void)
JS_DefineFile(const char *name, const char *script);

// Starts node.js engine instance
// !!Executes the main.js file
NWRAP_EXTERN(void)
JS_StartEngine(const char* home_folder);

// define a native method that can be called from JS land
// i.e.
// native code: JS_SetProcessNative("testMethod", my_callback);
// js code: process.natives.testMethod(1, true);
NWRAP_EXTERN(void)
JS_SetProcessNative(const char *name, JS_CALLBACK callback);

// define a native method that can be called from JS land
// i.e.
// native code: JS_DefineMethod(obj, "testMethod", my_callback);
// js code: obj.testMethod(1, true);
NWRAP_EXTERN(void)
JS_SetNativeMethod(JS_Value &obj, const char *name, JS_CALLBACK callback);

// loop io events for once. If there is any IO left to do
// this method returns 1 otherwise 0
NWRAP_EXTERN(int)
JS_LoopOnce();

// loop io events until they are finished
NWRAP_EXTERN(int)
JS_Loop();

// quit event loop
NWRAP_EXTERN(void)
JS_QuitLoop();

NWRAP_EXTERN(bool)
JS_IsSpiderMonkey();

NWRAP_EXTERN(bool)
JS_IsV8();

NWRAP_EXTERN(bool)
JS_IsChakra();

// Destroys node.js engine
NWRAP_EXTERN(void)
JS_StopEngine();

#ifdef __cplusplus
}
#endif

#endif  // SRC_JS_HEADER_
