#include "public/jx.h"
#define JXCORE_WRAPPER
#include "../node/node_wrapper.h"
#include <stdlib.h>
#include <string>

NWRAP_EXTERN(bool)
JS_IsFunction(JS_Value &value) {
  return JX_IsFunction((JXValue*)&value);
}

NWRAP_EXTERN(bool)
JS_IsError(JS_Value &value) {
  return JX_IsError((JXValue*)&value);
}

NWRAP_EXTERN(bool)
JS_IsInt32(JS_Value &value) {
  return JX_IsInt32((JXValue*)&value);
}

NWRAP_EXTERN(bool)
JS_IsDouble(JS_Value &value) {
  return JX_IsDouble((JXValue*)&value);
}

NWRAP_EXTERN(bool)
JS_IsBoolean(JS_Value &value) {
  return JX_IsBoolean((JXValue*)&value);
}

NWRAP_EXTERN(bool)
JS_IsString(JS_Value &value) {
  return JX_IsString((JXValue*)&value);
}

NWRAP_EXTERN(bool)
JS_IsJSON(JS_Value &value) { return JX_IsObject((JXValue*)&value); }

NWRAP_EXTERN(bool)
JS_IsBuffer(JS_Value &value) {
  return JX_IsBuffer((JXValue*)&value);
}

NWRAP_EXTERN(bool)
JS_IsUndefined(JS_Value &value) {
  return JX_IsUndefined((JXValue*)&value);
}

NWRAP_EXTERN(bool)
JS_IsNull(JS_Value &value) {
  return JX_IsNull((JXValue*)&value);
}

NWRAP_EXTERN(bool)
JS_IsNullOrUndefined(JS_Value &value) {
  return JX_IsNullOrUndefined((JXValue*)&value);
}

NWRAP_EXTERN(bool)
JS_IsObject(JS_Value &value) {
  return JX_IsObject((JXValue*)&value);
}

NWRAP_EXTERN(int32_t)
JS_GetInt32(JS_Value &value) {
  return JX_GetInt32((JXValue*)&value);
}

NWRAP_EXTERN(double)
JS_GetDouble(JS_Value &value) {
  return JX_GetDouble((JXValue*)&value);
}

NWRAP_EXTERN(bool)
JS_GetBoolean(JS_Value &value) {
  return JX_GetBoolean((JXValue*)&value);
}

NWRAP_EXTERN(char *)
JS_GetString(JS_Value &value) {
  return JX_GetString((JXValue*)&value);
}

NWRAP_EXTERN(int32_t)
JS_GetDataLength(JS_Value &value) {
  return JX_GetDataLength((JXValue*)&value);
}

NWRAP_EXTERN(char *)
JS_GetBuffer(JS_Value &value) {
  return JX_GetBuffer((JXValue*)&value);
}

NWRAP_EXTERN(void)
JS_Free(JS_Value &value) {
  JX_Free((JXValue*)&value);
}

NWRAP_EXTERN(bool)
JS_CallFunction(JS_Value& fnc, JS_Value *params, const int argc, JS_Value *out) {
  return JX_CallFunction((JXValue*)&fnc, (JXValue*)params, argc, (JXValue*)out);
}

NWRAP_EXTERN(void)
JS_SetInt32(JS_Value &value, const int32_t val) {
  JX_SetInt32((JXValue*)&value, val);
}

NWRAP_EXTERN(void)
JS_SetDouble(JS_Value &value, const double val) {
  JX_SetDouble((JXValue*)&value, val);
}

NWRAP_EXTERN(void)
JS_SetBoolean(JS_Value &value, const bool val) {
  JX_SetBoolean((JXValue*)&value, val);
}

NWRAP_EXTERN(void)
JS_SetString(JS_Value &value, const char *val, const int32_t length) {
  JX_SetString((JXValue*)&value, val, length);
}

NWRAP_EXTERN(void)
JS_SetUCString(JS_Value &value, const uint16_t *val, const int32_t _length) {
  JX_SetUCString((JXValue*)&value, val, _length);
}

NWRAP_EXTERN(void)
JS_SetJSON(JS_Value &value, const char *val, const int32_t length) {
  JX_SetJSON((JXValue*)&value, val, length);
}

NWRAP_EXTERN(void)
JS_SetError(JS_Value &value, const char *val, const int32_t length) {
  JX_SetError((JXValue*)&value, val, length);
}

NWRAP_EXTERN(void)
JS_SetBuffer(JS_Value &value, const char *val, const int32_t length) {
  JX_SetBuffer((JXValue*)&value, val, length);
}

NWRAP_EXTERN(void)
JS_SetUndefined(JS_Value &value) { JX_SetUndefined((JXValue*)&value); }

NWRAP_EXTERN(void)
JS_SetNull(JS_Value &value) { JX_SetNull((JXValue*)&value); }

NWRAP_EXTERN(void)
JS_SetObject(JS_Value &value_to, JS_Value &value_from) {
  JX_SetObject((JXValue*)&value_to, (JXValue*)&value_from);
}

NWRAP_EXTERN(bool)
JS_MakePersistent(JS_Value &value) {
  return JX_MakePersistent((JXValue*)&value);
}

NWRAP_EXTERN(bool)
JS_ClearPersistent(JS_Value &value) {
  return JX_ClearPersistent((JXValue*)&value);
}

NWRAP_EXTERN(bool)
JS_New(JS_Value &value) {
  return JX_New((JXValue*)&value);
}

NWRAP_EXTERN(bool)
JS_CreateEmptyObject(JS_Value &value) {
  return JX_CreateEmptyObject((JXValue*)&value);
}

NWRAP_EXTERN(bool)
JS_CreateArrayObject(JS_Value &value) {
  return JX_CreateArrayObject((JXValue*)&value);
}

NWRAP_EXTERN(void)
JS_SetNamedProperty(JS_Value &object, const char *name, JS_Value &prop) {
  JX_SetNamedProperty((JXValue*)&object, name, (JXValue*)&prop);
}

NWRAP_EXTERN(void)
JS_SetIndexedProperty(JS_Value &object, const unsigned index, JS_Value &prop) {
  JX_SetIndexedProperty((JXValue*)&object, index, (JXValue*)&prop);
}

NWRAP_EXTERN(void)
JS_GetNamedProperty(JS_Value &object, const char *name, JS_Value &out) {
  JX_GetNamedProperty((JXValue*)&object, name, (JXValue*)&out);
}

NWRAP_EXTERN(void)
JS_GetIndexedProperty(JS_Value &object, const int index, JS_Value &out) {
  JX_GetIndexedProperty((JXValue*)&object, index, (JXValue*)&out);
}

NWRAP_EXTERN(void)
JS_GetGlobalObject(JS_Value &out) {
  JX_GetGlobalObject((JXValue*)&out);
}

NWRAP_EXTERN(void)
JS_GetProcessObject(JS_Value &out) {
  JX_GetProcessObject((JXValue*)&out);
}

NWRAP_EXTERN(void)
JS_WrapObject(JS_Value &object, void *ptr) {
  JX_WrapObject((JXValue*)&object, ptr);
}

NWRAP_EXTERN(void *)
JS_UnwrapObject(JS_Value &object) {
  return JX_UnwrapObject((JXValue*)&object);
}

void JS_SetProcessNative(const char *name, JS_CALLBACK callback) {
  JX_DefineExtension(name, (JX_CALLBACK)callback);
}

void JS_SetNativeMethod(JS_Value &value, const char *name,
                        JS_CALLBACK callback) {
  return JX_SetNativeMethod((JXValue*)&value, name, (JX_CALLBACK)callback);
}

bool JS_Evaluate(const char *data, const char *script_name, JS_Value &jxresult) {
  return JX_Evaluate(data, script_name, (JXValue*)&jxresult);
}

void JS_DefineMainFile(const char *data) {
  JX_DefineMainFile(data);
}

void JS_DefineFile(const char *name, const char *file) {
  JX_DefineFile(name, file);
}

void callback(JXValue *results, int argc) {
  // do nothing
}

NODE_INIT_CALLBACK init_callback = nullptr;

void JS_DefineNodeOnInitCallback(NODE_INIT_CALLBACK cb) {
  init_callback = cb;
}

char *path_copy;
void JS_StartEngine(const char* path) {
  const size_t len = strlen(path);
  path_copy = (char*) malloc(sizeof(char) * (len + 4));
  memcpy(path_copy, path, len);
  if (path[len-1] == '/' || path[len-1] == '\\') {
    memcpy(path_copy + len, "jx", 2);
    path_copy[len + 2] = char(0);
  } else {
    memcpy(path_copy + len, "/jx", 3);
    path_copy[len + 3] = char(0);
  }

  JX_Initialize(path_copy, callback);
  JX_InitializeNewEngine();
  
  if (init_callback != nullptr) {
    init_callback();
  }
  
  JX_StartEngine();
}

int JS_LoopOnce() {
  return JX_LoopOnce();
}

int JS_Loop() {
  return JX_Loop();
}

void JS_QuitLoop() {
  return JX_QuitLoop();
}

bool JS_IsV8() {
  return JX_IsV8();
}

bool JS_IsSpiderMonkey() {
  return JX_IsSpiderMonkey();
}

bool JS_IsChakra() {
  return JX_IsChakra();
}

void JS_StopEngine() {
  free(path_copy);
  JX_StopEngine();
}
