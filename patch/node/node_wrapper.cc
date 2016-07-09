// See LICENSE file

#include "node.h"
#include "node_wrapper.h"
#include "node_internal_wrapper.h"
#include <stdio.h>
#include <string>
#include <string.h>

// todo: UWP ?
#define warn_console printf

class ScopeWatch {
  static bool in_scope_;
  bool entered_;
  bool in_scope_faked_;
  v8::Isolate *isolate_;

 public:
  ScopeWatch(v8::Isolate *isolate, bool fake_in_scope = false)
    : isolate_(isolate), entered_(false), in_scope_faked_(fake_in_scope) {
    if (!IsInScope()) EnterScope();
  }

  static bool IsInScope() { return in_scope_; }

  bool EnteredNewScope() { return entered_; }

  void EnterScope() {
    if (!in_scope_faked_)
      isolate_->Enter();
    entered_ = true;
    in_scope_ = true;
  }

  ~ScopeWatch() {
    if (entered_) {
      if (!in_scope_faked_)
        isolate_->Exit();
      in_scope_ = false;
    }
  }
};

bool ScopeWatch::in_scope_ = false;
JS_PERSISTENT_FUNCTION JSONparse;

JS_HANDLE_VALUE JS_Parse(node::Environment *env, const char *str) {
  JS_ENTER_SCOPE_WITH(env->isolate());
  JS_DEFINE_STATE_MARKER(env);

  JS_LOCAL_STRING str_value = UTF8_TO_STRING(str);
  // Init
  if (JS_IS_EMPTY((JSONparse))) {
    JS_LOCAL_FUNCTION _JSONparse =
        JS_CAST_FUNCTION(JS_COMPILE_AND_RUN(STD_TO_STRING(
                                                "(function(str) {\n"
                                                "  try {\n"
                                                "    return JSON.parse(str);\n"
                                                "  } catch (e) {\n"
                                                "    return e;\n"
                                                "  }\n"
                                                "});"),
                                            STD_TO_STRING("binding:parse")));
    JS_NEW_PERSISTENT_FUNCTION(JSONparse, _JSONparse);
  }

  JS_LOCAL_VALUE args[1] = {str_value};

  JS_LOCAL_FUNCTION fncl = JS_TYPE_TO_LOCAL_FUNCTION(JSONparse);
  JS_LOCAL_VALUE result = JS_METHOD_CALL(fncl, JS_GET_GLOBAL(), 1, args);

  return JS_LEAVE_SCOPE(result);
}

bool ConvertToJSValue(node::Environment *env, JS_HANDLE_VALUE_REF ret_val,
                      JS_Value *result) {
  assert(result->com_ && "Object wasn't initialized");
  result->persistent_ = false;

  JS_DEFINE_STATE_MARKER(env);

  if (JS_IS_NULL(ret_val)) {
    result->type_ = RT_Null;
    return true;
  }

  if (JS_IS_UNDEFINED(ret_val)) {
    result->type_ = RT_Undefined;
    return true;
  }

  result->type_ = RT_Undefined;

  if (JS_IS_BOOLEAN(ret_val)) {
    result->type_ = RT_Boolean;
    result->size_ = sizeof(bool);
  } else if (JS_IS_NUMBER(ret_val)) {
    if (JS_IS_INT32(ret_val)) {
      result->type_ = RT_Int32;
      result->size_ = sizeof(int32_t);
    } else {
      result->type_ = RT_Double;
      result->size_ = sizeof(double);
    }
  } else if (JS_IS_BUFFER(ret_val)) {
    result->type_ = RT_Buffer;
    result->size_ = BUFFER__LENGTH(ret_val);
  } else if (JS_IS_STRING(ret_val)) {
    result->type_ = RT_String;
    JS_LOCAL_STRING str = JS_VALUE_TO_STRING(ret_val);
    result->size_ = JS_GET_STRING_LENGTH(str);
  }

  if (result->type_ != RT_Undefined) {
    JSValueWrapper *pr_wrap = new JSValueWrapper(env, ret_val);
    result->data_ = (void *)pr_wrap;
    return true;
  }

  if (JS_IS_FUNCTION(ret_val)) {
    JS_LOCAL_VALUE ud_val = JS_UNDEFINED();
    JS_LOCAL_FUNCTION fnc = JS_CAST_FUNCTION(JS_VALUE_TO_OBJECT(ret_val));
    JSFunctionWrapper *fnc_wrap = new JSFunctionWrapper(env, fnc, ud_val);
    result->data_ = fnc_wrap;
    result->size_ = sizeof(JSFunctionWrapper);
    result->type_ = RT_Function;

    return true;
  }

  if (!JS_IS_OBJECT(ret_val)) {
    result->type_ = RT_Error;
    JS_LOCAL_STRING dummy = STD_TO_STRING("Unsupported return type.");
    result->size_ = JS_GET_STRING_LENGTH(dummy);
    result->data_ = (void *)new JSValueWrapper(env, dummy);

    return false;
  }

  result->type_ = RT_Object;
  JSValueWrapper *wrap = new JSValueWrapper(env, ret_val);
  result->data_ = (void *)wrap;
  result->size_ = 1;

  return true;
}

NWRAP_EXTERN(bool)
JS_IsFunction(JS_Value &value) {
  return value.size_ > 0 && value.type_ == RT_Function;
}

NWRAP_EXTERN(bool)
JS_IsError(JS_Value &value) {
  return value.size_ > 0 && value.type_ == RT_Error;
}

NWRAP_EXTERN(bool)
JS_IsInt32(JS_Value &value) {
  return value.size_ > 0 && value.type_ == RT_Int32;
}

NWRAP_EXTERN(bool)
JS_IsDouble(JS_Value &value) {
  return value.size_ > 0 && value.type_ == RT_Double;
}

NWRAP_EXTERN(bool)
JS_IsBoolean(JS_Value &value) {
  return value.size_ > 0 && value.type_ == RT_Boolean;
}

NWRAP_EXTERN(bool)
JS_IsString(JS_Value &value) { return value.type_ == RT_String; }

NWRAP_EXTERN(bool)
JS_IsJSON(JS_Value &value) { return JS_IsObject(value); }

NWRAP_EXTERN(bool)
JS_IsBuffer(JS_Value &value) {
  return value.size_ > 0 && value.type_ == RT_Buffer;
}

NWRAP_EXTERN(bool)
JS_IsUndefined(JS_Value &value) { return value.type_ == RT_Undefined; }

NWRAP_EXTERN(bool)
JS_IsNull(JS_Value &value) { return value.type_ == RT_Null; }

NWRAP_EXTERN(bool)
JS_IsNullOrUndefined(JS_Value &value) {
  return value.type_ == RT_Null || value.type_ == RT_Undefined;
}

NWRAP_EXTERN(bool)
JS_IsObject(JS_Value &value) { return value.type_ == RT_Object; }

NWRAP_EXTERN(int32_t)
JS_GetInt32(JS_Value &value) {
  EMPTY_CHECK(0);

  UNWRAP_RESULT(value.data_);
  UNWRAP_COM(value);
  assert(env && "Did you call JS_StartEngine ?");

  int32_t ret;

  RUN_IN_SCOPE({ ret = INT32_TO_STD(JS_TYPE_TO_LOCAL_VALUE(wrap->value_)); });

  return ret;
}

NWRAP_EXTERN(double)
JS_GetDouble(JS_Value &value) {
  EMPTY_CHECK(0);

  UNWRAP_RESULT(value.data_);
  UNWRAP_COM(value);
  assert(env && "Did you call JS_StartEngine ?");

  double ret;

  RUN_IN_SCOPE({ ret = NUMBER_TO_STD(JS_TYPE_TO_LOCAL_VALUE(wrap->value_)); });

  return ret;
}

NWRAP_EXTERN(bool)
JS_GetBoolean(JS_Value &value) {
  EMPTY_CHECK(false);

  UNWRAP_RESULT(value.data_);
  UNWRAP_COM(value);
  assert(env && "Did you call JS_StartEngine ?");

  bool ret;

  RUN_IN_SCOPE({ ret = BOOLEAN_TO_STD(JS_TYPE_TO_LOCAL_VALUE(wrap->value_)); });

  return ret;
}

NWRAP_EXTERN(char *)
JS_GetString(JS_Value &value) {
  EMPTY_CHECK(0);

  UNWRAP_COM(value);
  UNWRAP_RESULT(value.data_);
  assert(env && "Did you call JS_StartEngine ?");

  char *ret;
  RUN_IN_SCOPE({
    switch (value.type_) {
      case RT_String: {
        JS_LOCAL_OBJECT objl = JS_OBJECT_FROM_PERSISTENT(wrap->value_);
        ret = strdup(STRING_TO_STD(JS_VALUE_TO_STRING(objl)));
      } break;
      case RT_Error:
      case RT_Object: {
        JS_LOCAL_OBJECT obj = JS_OBJECT_FROM_PERSISTENT(wrap->value_);
        ret = strdup(STRING_TO_STD(obj));

        bool get_message = false;
        JS_LOCAL_STRING str_msg;
        if (strlen(ret) == 2 && ret[1] == '}') {
          str_msg = STD_TO_STRING("message");
          if (JS_HAS_NAME(obj, str_msg)) {
            free(ret);
            get_message = true;
          }
        }

        if (get_message) {
          JS_LOCAL_VALUE msg = JS_GET_NAME(obj, str_msg);
          JS_LOCAL_VALUE name = JS_GET_NAME(obj, STD_TO_STRING("name"));

          std::string err_msg;
          err_msg += STRING_TO_STD(name);
          err_msg += ": ";
          err_msg += STRING_TO_STD(msg);
          ret = strdup(err_msg.c_str());
        }
      } break;
      default: {
        JS_LOCAL_OBJECT objl = JS_OBJECT_FROM_PERSISTENT(wrap->value_);
        // calls JavaScript .toString
        ret = strdup(STRING_TO_STD(JS_VALUE_TO_STRING(objl)));
      }
    }
  });

  return ret;
}

NWRAP_EXTERN(int32_t)
JS_GetDataLength(JS_Value &value) {
  EMPTY_CHECK(0);

  return value.size_;
}

NWRAP_EXTERN(char *)
JS_GetBuffer(JS_Value &value) {
  EMPTY_CHECK(NULL);

  UNWRAP_COM(value);
  UNWRAP_RESULT(value.data_);
  assert(env && "Did you call JS_StartEngine ?");

  char *data = NULL;
  RUN_IN_SCOPE({
    if (value.type_ == RT_Buffer) {
      JS_LOCAL_OBJECT obj = JS_OBJECT_FROM_PERSISTENT(wrap->value_);
      data = BUFFER__DATA(obj);
    }
  });

  return data;
}

NWRAP_EXTERN(void)
JS_Free(JS_Value &value) {
  if (value.persistent_) return;

  UNWRAP_COM(value);

  if (value.data_ == NULL || value.type_ == RT_Undefined ||
      value.type_ == RT_Null)
    return;

  RUN_IN_SCOPE({
    if (value.type_ == RT_Function) {
      assert(sizeof(JSFunctionWrapper) == value.size_ &&
             "Broken JS_Value Function Object");

      JSFunctionWrapper *wrap = (JSFunctionWrapper *)value.data_;

      wrap->Dispose();
      delete (wrap);
    } else {
      _FREE_MEM_(value.data_);
    }
  });
  value.data_ = NULL;
  value.size_ = 0;
  value.type_ = RT_Undefined;
}

NWRAP_EXTERN(bool)
JS_CallFunction(JS_Value &fnc, JS_Value *params, const int argc,
                JS_Value *out) {
  UNWRAP_COM(fnc);

  out->com_ = fnc.com_;
  out->data_ = NULL;
  out->size_ = 0;
  out->type_ = RT_Undefined;

  if (fnc.type_ != RT_Function || env == NULL ||
      sizeof(JSFunctionWrapper) != fnc.size_) {
    return false;
  }

  JSFunctionWrapper *wrap = (JSFunctionWrapper *)fnc.data_;

  bool done = false;
  bool ret;

  JS_HANDLE_VALUE res;
  RUN_IN_SCOPE({
    JS_HANDLE_VALUE *arr =
        (JS_HANDLE_VALUE *)malloc(sizeof(JS_HANDLE_VALUE) * argc);

    for (int i = 0; i < argc; i++) {
      if (params[i].type_ == RT_Undefined || params[i].type_ == RT_Null ||
          params[i].data_ == NULL) {
        arr[i] = params[i].type_ == RT_Undefined ? JS_UNDEFINED() : JS_NULL();
      } else if (params[i].type_ == RT_Function) {
        JSFunctionWrapper *fnc_wrap = (JSFunctionWrapper *)params[i].data_;
        arr[i] = fnc_wrap->GetFunction();
      } else {
        JSValueWrapper *wrap = (JSValueWrapper *)params[i].data_;
        arr[i] = JS_TYPE_TO_LOCAL_VALUE(wrap->value_);
      }
    }
    res = wrap->Call(argc, arr, &done);
    free(arr);

    if (!done) {
      ret = false;
    } else {
      ret = ConvertToJSValue(env, res, out);
    }
  });

  return ret;
}

NWRAP_EXTERN(void)
JS_SetInt32(JS_Value &value, const int32_t val) {
  UNWRAP_COM(value);
  UNWRAP_RESULT(value.data_);
  assert(env && "Did you call JS_StartEngine ?");

  if (wrap == NULL) {
    wrap = new JSValueWrapper();
    value.data_ = (void *)wrap;
  } else if (!JS_IS_EMPTY(wrap->value_)) {
    JS_CLEAR_PERSISTENT(wrap->value_);
  }

  value.type_ = RT_Int32;
  value.size_ = sizeof(int32_t);

  RUN_IN_SCOPE({ JS_NEW_PERSISTENT_VALUE(wrap->value_, STD_TO_INTEGER(val)); });
}

NWRAP_EXTERN(void)
JS_SetDouble(JS_Value &value, const double val) {
  UNWRAP_COM(value);
  UNWRAP_RESULT(value.data_);
  assert(env && "Did you call JS_StartEngine ?");

  if (wrap == NULL) {
    wrap = new JSValueWrapper();
    value.data_ = (void *)wrap;
  } else if (!JS_IS_EMPTY(wrap->value_)) {
    JS_CLEAR_PERSISTENT(wrap->value_);
  }

  value.type_ = RT_Double;
  value.size_ = sizeof(double);

  RUN_IN_SCOPE({ JS_NEW_PERSISTENT_VALUE(wrap->value_, STD_TO_NUMBER(val)); });
}

NWRAP_EXTERN(void)
JS_SetBoolean(JS_Value &value, const bool val) {
  UNWRAP_COM(value);
  UNWRAP_RESULT(value.data_);
  assert(env && "Did you call JS_StartEngine ?");

  if (wrap == NULL) {
    wrap = new JSValueWrapper();
    value.data_ = (void *)wrap;
  } else if (!JS_IS_EMPTY(wrap->value_)) {
    JS_CLEAR_PERSISTENT(wrap->value_);
  }

  value.type_ = RT_Boolean;
  value.size_ = sizeof(bool);

  RUN_IN_SCOPE({ JS_NEW_PERSISTENT_VALUE(wrap->value_, STD_TO_BOOLEAN(val)); });
}

inline void SetString(JS_ValueType valueType, JS_Value &value, const char *val,
                      const int32_t length) {
  UNWRAP_COM(value);
  UNWRAP_RESULT(value.data_);
  assert(env && "Did you call JS_StartEngine ?");

  if (wrap == 0) {
    wrap = new JSValueWrapper();
    value.data_ = (void *)wrap;
  } else {
    JS_CLEAR_PERSISTENT(wrap->value_);
  }

  value.type_ = valueType;
  value.size_ = length;

  RUN_IN_SCOPE({
    JS_LOCAL_STRING str_val = UTF8_TO_STRING(val);
    JS_NEW_PERSISTENT_VALUE(wrap->value_, str_val);
  });
}

NWRAP_EXTERN(void)
JS_SetString(JS_Value &value, const char *val, const int32_t length) {
  SetString(RT_String, value, val, length);
}

NWRAP_EXTERN(void)
JS_SetJSON(JS_Value &value, const char *val, const int32_t length) {
  UNWRAP_COM(value);
  UNWRAP_RESULT(value.data_);
  assert(env && "Did you call JS_StartEngine ?");

  if (wrap == NULL) {
    wrap = new JSValueWrapper();
    value.data_ = (void *)wrap;
  } else if (!JS_IS_EMPTY(wrap->value_)) {
    JS_CLEAR_PERSISTENT(wrap->value_);
  }

  value.type_ = RT_Object;
  value.size_ = length == 0 ? strlen(val) : length;

  RUN_IN_SCOPE({
    JS_HANDLE_VALUE hval = JS_Parse(env, val);
    JS_NEW_PERSISTENT_VALUE(wrap->value_, hval);
  });
}

NWRAP_EXTERN(void)
JS_SetError(JS_Value &value, const char *val, const int32_t length) {
  // todo: create an error object
  SetString(RT_Error, value, val, length);
}

NWRAP_EXTERN(void)
JS_SetBuffer(JS_Value &value, const char *val, const int32_t length) {
  UNWRAP_COM(value);
  UNWRAP_RESULT(value.data_);
  assert(env && "Did you call JS_StartEngine ?");

  if (wrap == NULL) {
    wrap = new JSValueWrapper();
    value.data_ = (void *)wrap;
  } else if (!JS_IS_EMPTY(wrap->value_)) {
    JS_CLEAR_PERSISTENT(wrap->value_);
  }

  value.type_ = RT_Buffer;
  value.size_ = (length == 0 && val != NULL) ? strlen(val) : length;

  RUN_IN_SCOPE({
    char *data = strdup(val);
    v8::Local<v8::Object> buff =
        node::Buffer::New(env, data, length).ToLocalChecked();
    JS_NEW_PERSISTENT_VALUE(wrap->value_, buff);
  });
}

NWRAP_EXTERN(void)
JS_SetUndefined(JS_Value &value) { value.type_ = RT_Undefined; }

NWRAP_EXTERN(void)
JS_SetNull(JS_Value &value) { value.type_ = RT_Null; }

NWRAP_EXTERN(void)
JS_SetObject(JS_Value &value_to, JS_Value &value_from) {
  UNWRAP_COM(value_to);
  UNWRAP_RESULT(value_to.data_);
  assert(env && "Did you call JS_StartEngine ?");

  assert(value_from.type_ == RT_Object && "value_from must be an Object");

  if (wrap == NULL) {
    wrap = new JSValueWrapper();
    value_to.data_ = (void *)wrap;
  } else if (!JS_IS_EMPTY(wrap->value_)) {
    JS_CLEAR_PERSISTENT(wrap->value_);
  }

  value_to.type_ = RT_Object;
  value_to.size_ = 1;

  JSValueWrapper *wrap_from = (JSValueWrapper *)value_from.data_;
  RUN_IN_SCOPE({ JS_NEW_PERSISTENT_VALUE(wrap->value_, wrap_from->value_); });
  value_to.persistent_ = false;
}

NWRAP_EXTERN(bool)
JS_MakePersistent(JS_Value &value) {
  assert(value.com_ != NULL && value.type_ != RT_Undefined &&
         value.type_ != RT_Null &&
         "Empty, Null or Undefined JS Value can not be persistent");

  bool pre = value.persistent_;
  value.persistent_ = true;

  return !pre;
}

NWRAP_EXTERN(bool)
JS_ClearPersistent(JS_Value &value) {
  assert(value.com_ != NULL && value.type_ != RT_Undefined &&
         value.type_ != RT_Null &&
         "Empty, Null or Undefined JS Value can not be persistent");

  bool pre = value.persistent_;
  value.persistent_ = false;

  return pre;
}

#define SET_UNDEFINED(to_)  \
  to_.type_ = RT_Undefined; \
  to_.data_ = NULL;         \
  to_.size_ = 0

NWRAP_EXTERN(bool)
JS_New(JS_Value &value) {
  node::Environment *env = node::__GetNodeEnvironment();
  assert(env && "Did you call JS_StartEngine ?");

  value.com_ = env;
  value.persistent_ = false;
  SET_UNDEFINED(value);

  return true;
}

NWRAP_EXTERN(bool)
JS_CreateEmptyObject(JS_Value &value) {
  node::Environment *env = node::__GetNodeEnvironment();
  assert(env && "Did you call JS_StartEngine ?");

  if (env == NULL) return false;
  JS_DEFINE_STATE_MARKER(env);

  value.com_ = env;

  JSValueWrapper *wrap = new JSValueWrapper();
  ScopeWatch watcher(env->isolate());

  RUN_IN_SCOPE(
      { JS_NEW_PERSISTENT_OBJECT(wrap->value_, JS_NEW_EMPTY_OBJECT()); });
  value.data_ = wrap;

  value.size_ = 1;
  value.persistent_ = false;
  value.type_ = RT_Object;

  return true;
}

NWRAP_EXTERN(bool)
JS_CreateArrayObject(JS_Value &value) {
  node::Environment *env = node::__GetNodeEnvironment();
  assert(env && "Did you call JS_StartEngine ?");

  if (env == NULL) return false;
  JS_DEFINE_STATE_MARKER(env);

  value.com_ = env;
  ScopeWatch watcher(env->isolate());
  JSValueWrapper *wrap = new JSValueWrapper();
  RUN_IN_SCOPE({ JS_NEW_PERSISTENT_OBJECT(wrap->value_, JS_NEW_ARRAY()); });

  value.data_ = wrap;
  value.size_ = 1;
  value.persistent_ = false;
  value.type_ = RT_Object;

  return true;
}

NWRAP_EXTERN(void)
JS_SetNamedProperty(JS_Value &object, const char *name, JS_Value &prop) {
  UNWRAP_COM(object);
  UNWRAP_RESULT(object.data_);
  assert(env && "Did you call JS_StartEngine ?");

  assert(object.type_ == RT_Object && "object must be an Object");

  JSValueWrapper *wrap_prop = NULL;

  if (prop.type_ != RT_Undefined && prop.type_ != RT_Null &&
      prop.data_ != NULL) {
    wrap_prop = (JSValueWrapper *)prop.data_;
  }

  RUN_IN_SCOPE({
    JS_LOCAL_VALUE val = wrap_prop != NULL
                             ? JS_TYPE_TO_LOCAL_VALUE(wrap_prop->value_)
                             : JS_NULL();
    JS_LOCAL_OBJECT obj = JS_OBJECT_FROM_PERSISTENT(wrap->value_);
    JS_NAME_SET(obj, STD_TO_STRING(name), val);
  });
}

NWRAP_EXTERN(void)
JS_SetIndexedProperty(JS_Value &object, const unsigned index, JS_Value &prop) {
  UNWRAP_COM(object);
  UNWRAP_RESULT(object.data_);
  assert(env && "Did you call JS_StartEngine ?");

  assert(object.type_ == RT_Object && "object must be an Object");

  JSValueWrapper *wrap_prop = NULL;

  if (prop.type_ != RT_Undefined && prop.type_ != RT_Null &&
      prop.data_ != NULL) {
    wrap_prop = (JSValueWrapper *)prop.data_;
  }

  RUN_IN_SCOPE({
    JS_LOCAL_VALUE val = wrap_prop != NULL
                             ? JS_TYPE_TO_LOCAL_VALUE(wrap_prop->value_)
                             : JS_NULL();
    JS_LOCAL_OBJECT obj = JS_OBJECT_FROM_PERSISTENT(wrap->value_);
    JS_INDEX_SET(obj, index, val);
  });
}

NWRAP_EXTERN(void)
JS_GetNamedProperty(JS_Value &object, const char *name, JS_Value &out) {
  UNWRAP_COM(object);
  UNWRAP_RESULT(object.data_);
  assert(env && "Did you call JS_StartEngine ?");

  assert(object.type_ == RT_Object && "object must be an Object");

  RUN_IN_SCOPE({
    JS_LOCAL_OBJECT obj = JS_OBJECT_FROM_PERSISTENT(wrap->value_);
    JS_LOCAL_VALUE sub_obj;
    if (JS_HAS_NAME(obj, STD_TO_STRING(name)))
      sub_obj = JS_GET_NAME(obj, STD_TO_STRING(name));
    else
      sub_obj = JS_UNDEFINED();

    out.data_ = NULL;
    out.size_ = 0;
    out.com_ = env;
    out.persistent_ = false;
    ConvertToJSValue(env, sub_obj, &out);
  });
}

NWRAP_EXTERN(void)
JS_GetIndexedProperty(JS_Value &object, const int index, JS_Value &out) {
  UNWRAP_COM(object);
  UNWRAP_RESULT(object.data_);
  assert(env && "Did you call JS_StartEngine ?");

  assert(object.type_ == RT_Object && "object must be an Object");

  RUN_IN_SCOPE({
    JS_LOCAL_OBJECT obj = JS_OBJECT_FROM_PERSISTENT(wrap->value_);
    JS_LOCAL_VALUE sub_obj = JS_GET_INDEX(obj, index);

    out.data_ = NULL;
    out.size_ = 0;
    out.com_ = env;
    out.persistent_ = false;
    ConvertToJSValue(env, sub_obj, &out);
  });
}

NWRAP_EXTERN(int)
JS_GetThreadIdByValue(JS_Value &value) { return 0; }

NWRAP_EXTERN(void)
JS_GetGlobalObject(JS_Value &out) {
  node::Environment *env = node::__GetNodeEnvironment();
  assert(env && "Did you call JS_StartEngine ?");

  JS_DEFINE_STATE_MARKER(env);
  ScopeWatch watcher(__contextORisolate);

  RUN_IN_SCOPE({
    JS_LOCAL_OBJECT obj = JS_GET_GLOBAL();

    out.data_ = NULL;
    out.size_ = 0;
    out.com_ = env;
    out.persistent_ = false;
    ConvertToJSValue(env, obj, &out);
  });
}

NWRAP_EXTERN(void)
JS_GetProcessObject(JS_Value &out) {
  node::Environment *env = node::__GetNodeEnvironment();
  assert(env && "Did you call JS_StartEngine ?");

  JS_DEFINE_STATE_MARKER(env);
  ScopeWatch watcher(__contextORisolate);

  RUN_IN_SCOPE({
    JS_HANDLE_OBJECT obj = env->process_object();

    out.data_ = NULL;
    out.size_ = 0;
    out.com_ = env;
    out.persistent_ = false;
    ConvertToJSValue(env, obj, &out);
  });
}

NWRAP_EXTERN(void)
JS_WrapObject(JS_Value &object, void *ptr) {
  UNWRAP_COM(object);
  UNWRAP_RESULT(object.data_);
  assert(env && "Did you call JS_StartEngine ?");

  assert(object.type_ == RT_Object && "object must be an Object");

  RUN_IN_SCOPE({
    JS_LOCAL_OBJECT obj = JS_OBJECT_FROM_PERSISTENT(wrap->value_);

    JS_SET_POINTER_DATA(obj, ptr);
  });
}

NWRAP_EXTERN(void *)
JS_UnwrapObject(JS_Value &object) {
  UNWRAP_COM(object);
  UNWRAP_RESULT(object.data_);
  assert(env && "Did you call JS_StartEngine ?");

  assert(object.type_ == RT_Object && "object must be an Object");

  RUN_IN_SCOPE({
    JS_LOCAL_OBJECT obj = JS_OBJECT_FROM_PERSISTENT(wrap->value_);

    return JS_GET_POINTER_DATA(obj);
  });

  // make compiler happy
  return NULL;
}

// allocates one extra JS_Value memory at the end of the array
// Uses that one for a return value
#define CONVERT_ARG_TO_RESULT(results, context)                 \
  JS_Value *results = NULL;                                     \
  const int len = args.Length() - start_arg;                    \
  {                                                             \
    results = (JS_Value *)malloc(sizeof(JS_Value) * (len + 1)); \
    for (int i = 0; i < len; i++) {                             \
      JS_HANDLE_VALUE val = args[i + start_arg];                \
      results[i].com_ = context;                                \
      results[i].data_ = NULL;                                  \
      results[i].size_ = 0;                                     \
      results[i].type_ = RT_Undefined;                          \
      ConvertToJSValue(env, val, &results[i]);                  \
    }                                                           \
    results[len].com_ = context;                                \
    results[len].data_ = NULL;                                  \
    results[len].size_ = 0;                                     \
    results[len].type_ = RT_Undefined;                          \
  }

static int extension_id = 0;
#define MAX_CALLBACK_ID 1024
struct CallbackHost {
  char* name;
  JS_CALLBACK callback;
  CallbackHost():name(NULL) {}
  ~CallbackHost() { free(name); }
};

static CallbackHost callbacks[MAX_CALLBACK_ID];

JS_LOCAL_METHOD(extensionCallback) {
  if (args.Length() == 0 || !args[0]->IsUint32()) {
    THROW_EXCEPTION("This method expects the first parameter is unsigned");
  }

  const int interface_id = INTEGER_TO_STD(args[0]);
  if (interface_id >= MAX_CALLBACK_ID || callbacks[interface_id].name == NULL)
    THROW_EXCEPTION("There is no extension method for given Id");

  const int start_arg = 1;
  CONVERT_ARG_TO_RESULT(results, env);

  callbacks[interface_id].callback(results, len);

  for (int i = 0; i < len; i++) {
    JS_Free(results[i]);
  }

  if (results[len].type_ != RT_Undefined) {
    assert((results[len].data_ != NULL ||
            (results[len].size_ == 0 && results[len].type_ == RT_Buffer)) &&
           "Result value is NULL and it is not a zero length buffer");
    assert((results[len].size_ != 0 || (results[len].type_ == RT_String ||
                                        results[len].type_ == RT_Buffer)) &&
           "Return value was corrupted");

    if (results[len].type_ == RT_Error) {
      char *cmsg = JS_GetString(results[len]);
      std::string msg = cmsg;
      free(cmsg);
      JS_Free(results[len]);
      THROW_EXCEPTION(msg.c_str());
    } else if (results[len].type_ == RT_Function) {
      assert(sizeof(JSFunctionWrapper) == results[len].size_ &&
             "Type mixing? This can not be a Function");

      JSFunctionWrapper *fnc_wrap = (JSFunctionWrapper *)results[len].data_;
      JS_HANDLE_FUNCTION fnc = fnc_wrap->GetFunction();
      JS_Free(results[len]);

      RETURN_PARAM(fnc);
    } else if (results[len].type_ == RT_Null) {
      JS_Free(results[len]);
      RETURN_PARAM(JS_NULL());
    }

    JSValueWrapper *wrap = (JSValueWrapper *)results[len].data_;
    JS_HANDLE_VALUE ret_val = JS_TYPE_TO_LOCAL_VALUE(wrap->value_);
    JS_Free(results[len]);
    RETURN_PARAM(ret_val);
  }

  free(results);
}
JS_METHOD_END

void DeclareProxy(node::Environment *env, JS_HANDLE_OBJECT_REF methods,
                  const char *name, int interface_id,
                  JS_NATIVE_METHOD native_method) {
  JS_ENTER_SCOPE_WITH(env->isolate());
  JS_DEFINE_STATE_MARKER(env);

  char *script = (char *)malloc(2048);
  if (script == nullptr) {
    env->ThrowError("Out of memory");
    return;
  }

  int len = sprintf(script,
                    "(function(target, host) {\n"
                    "  host.%s = function() {\n"
                    "    var arr = [];\n"
                    "    arr[0] = %d;\n"
                    "    for (var i=0,ln=arguments.length; i<ln; i++) {\n"
                    "      arr[i+1] = arguments[i];\n"
                    "    }\n"
                    "    return host.%s.ref.apply(null, arr);\n"
                    "  };\n"
                    "  host.%s.ref = target.%s;\n"
                    "})",
                    name, interface_id, name, name, name);

  assert(len < 2048);
  script[len] = '\0';

  JS_LOCAL_FUNCTION defineProxy = JS_CAST_FUNCTION(JS_COMPILE_AND_RUN(
      STD_TO_STRING(script), STD_TO_STRING("proxy_method:script")));
  free(script);

  JS_LOCAL_OBJECT obj = JS_NEW_EMPTY_OBJECT();
  NODE_SET_METHOD(obj, name, native_method);

  JS_LOCAL_VALUE args[2] = {obj, JS_TYPE_TO_LOCAL_VALUE(methods)};
  JS_METHOD_CALL(defineProxy, JS_GET_GLOBAL(), 2, args);
}

void DefineProxyMethod(JS_HANDLE_OBJECT obj, const char *name,
                       const int interface_id, JS_NATIVE_METHOD method) {
  node::Environment *env = node::__GetNodeEnvironment();
  assert(env && "Did you call JS_StartEngine ?");

  JS_LOCAL_OBJECT natives = JS_VALUE_TO_OBJECT(obj);

  DeclareProxy(env, natives, name, interface_id, method);
}

void JS_SetNativeMethod(JS_Value &value, const char *name,
                        JS_CALLBACK callback) {
  int id = extension_id++;
  assert(id < MAX_CALLBACK_ID &&
         "Maximum amount of extension methods reached.");
  callbacks[id].callback = callback;
  callbacks[id].name = strdup(name);

  UNWRAP_RESULT(value.data_);
  UNWRAP_COM(value);
  assert(env && "Did you call JS_StartEngine ?");

  RUN_IN_SCOPE({
    JS_LOCAL_OBJECT objl = JS_OBJECT_FROM_PERSISTENT(wrap->value_);
    DefineProxyMethod(objl, name, id, extensionCallback);
  });
}

void JS_SetProcessNativeInternal(int id, const char *name, JS_CALLBACK callback) {
  node::Environment *env = node::__GetNodeEnvironment();
  if (env) {
    JS_DEFINE_STATE_MARKER(env);
    ScopeWatch watcher(env->isolate());

    RUN_IN_SCOPE({
      JS_HANDLE_OBJECT process = env->process_object();
      JS_LOCAL_STRING strNatives = STD_TO_STRING("natives");

      if (!JS_HAS_NAME(process, strNatives)) {
        JS_HANDLE_OBJECT natives_ = JS_NEW_EMPTY_OBJECT();
        JS_NAME_SET(process, strNatives, natives_);
      }

      JS_HANDLE_OBJECT natives =
          JS_VALUE_TO_OBJECT(JS_GET_NAME(process, strNatives));

      DefineProxyMethod(natives, name, id, extensionCallback);
    });
  }
}

void JS_SetProcessNative(const char *name, JS_CALLBACK callback) {
  int id = extension_id++;
  assert(id < MAX_CALLBACK_ID &&
         "Maximum amount of extension methods reached.");
  callbacks[id].callback = callback;
  callbacks[id].name = strdup(name);

  JS_SetProcessNativeInternal(id, name, callback);
}

#define MANAGE_EXCEPTION                             \
  JS_LOCAL_VALUE exception = try_catch.Exception();  \
  result.type_ = RT_Error;                           \
  result.data_ = new JSValueWrapper(env, exception); \
  result.size_ = 1;                                  \
  return true;

bool Evaluate(const char *source, const char *filename, JS_Value &result) {
  node::Environment *env = node::__GetNodeEnvironment();
  JS_DEFINE_STATE_MARKER(env);

  SET_UNDEFINED(result);
  result.com_ = env;

  JS_LOCAL_STRING source_ = UTF8_TO_STRING(source);
  JS_LOCAL_STRING filename_ = UTF8_TO_STRING(filename);

  JS_TRY_CATCH(try_catch);

  JS_LOCAL_SCRIPT script = JS_SCRIPT_COMPILE(source_, filename_);

  if (JS_IS_EMPTY(script)) {
    MANAGE_EXCEPTION
  }

  JS_LOCAL_VALUE scr_return = JS_SCRIPT_RUN(script);

  if (try_catch.HasCaught()) {
    MANAGE_EXCEPTION
  }

  return ConvertToJSValue(env, scr_return, &result);
}

bool JS_Evaluate(const char *data, const char *script_name, JS_Value &out) {
  const char *name = script_name == NULL ? "JS_Evaluate" : script_name;
  node::Environment *env = node::__GetNodeEnvironment();
  assert(env && "Did you call JS_StartEngine ?");

  JS_DEFINE_STATE_MARKER(env);
  ScopeWatch watcher(__contextORisolate);

  RUN_IN_SCOPE({ return Evaluate(data, name, out); });
}

void JS_DefineMainFile(const char *data) {
  node::AddFile("main.js", strdup(data), strlen(data));
}

void JS_DefineFile(const char *name, const char *file) {
  node::AddFile(name, strdup(file), strlen(file));
}

int JS_LoopOnce() { return node::__Loop(true); }

int JS_Loop() { return node::__Loop(false); }

void DEFINE_NATIVES__() {
  for(int i=0; i < extension_id; i++) {
    JS_SetProcessNativeInternal(i, callbacks[i].name, callbacks[i].callback);
  }
}

char *copy_argv = NULL;
char *app_args[2];
void JS_StartEngine(const char* home_folder) {
  assert(copy_argv == NULL && "This shouldn't be called twice!");

#if defined(__IOS__) || defined(__ANDROID__) || defined(DEBUG)
  warn_console("Node engine is starting\n");
#endif
  size_t home_length = strlen(home_folder);
  copy_argv = (char *)malloc((14 + home_length) * sizeof(char));
  memcpy(copy_argv, home_folder, home_length * sizeof(char));
  if (home_length && home_folder[home_length - 1] != '/' &&
      home_folder[home_length - 1] != '\\') {
    memcpy(copy_argv + home_length, "/node\0main.js", 13 * sizeof(char));
    copy_argv[home_length + 13] = '\0';
    app_args[1] = copy_argv + home_length + 6;
  } else {
    memcpy(copy_argv + home_length, "node\0main.js", 12 * sizeof(char));
    copy_argv[home_length + 12] = '\0';
    app_args[1] = copy_argv + home_length + 5;
  }
  
  app_args[0] = copy_argv;

  node::__Start(2, app_args, DEFINE_NATIVES__);

  JS_LoopOnce();
}

void JS_QuitLoop() {
  node::Environment* env = node::__GetNodeEnvironment();
  assert(env && "Did you call JS_StartEngine ?");

  uv_stop(env->isolate_data()->event_loop());
}

bool JS_IsV8() {
#ifndef JS_ENGINE_CHAKRA
  return true;
#else
  return false;
#endif
}

bool JS_IsSpiderMonkey() { return false; }

bool JS_IsChakra() {
#ifdef JS_ENGINE_CHAKRA
  return true;
#else
  return false;
#endif
}

void JS_StopEngine() {
  node::__Shutdown();

#if defined(__IOS__) || defined(__ANDROID__) || defined(DEBUG)
  warn_console("Node engine has destroyed\n");
#endif
}
