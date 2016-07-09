// See LICENSE file

#ifndef SRC_NODE_WRAPPER_H
#define SRC_NODE_WRAPPER_H

#include "node.h"
#include "env.h"
#include "env-inl.h"

#define GET_ISOLATE(env) env->isolate()

#define JS_V8_ARGUMENT v8::FunctionCallbackInfo<v8::Value>

typedef void (*JS_NATIVE_METHOD)(const JS_V8_ARGUMENT &args);
#define JS_ENGINE_MARKER v8::Isolate *

#define JS_DEFINE_STATE_MARKER(x) \
  JS_ENGINE_MARKER __contextORisolate = GET_ISOLATE(x)

#define JS_CURRENT_CONTEXT() env->context()

#define JS_GET_GLOBAL() JS_CURRENT_CONTEXT()->Global()

#define JS_LEAVE_SCOPE(x) scope.Escape(x)

#define JS_GET_STATE_MARKER() __contextORisolate

// !! V8_T_ members are private macros
#define V8_T_VALUE v8::Value
#define V8_T_OBJECT v8::Object
#define V8_T_OBJECT_TEMPLATE v8::ObjectTemplate
#define V8_T_STRING v8::String
#define V8_T_SCRIPT v8::Script
#define V8_T_ARRAY v8::Array
#define V8_T_BOOLEAN v8::Boolean
#define V8_T_INTEGER v8::Integer
#define V8_T_NUMBER v8::Number
#define V8_T_FUNCTION v8::Function
#define V8_T_FUNCTION_TEMPLATE v8::FunctionTemplate
#define V8_T_UNDEFINED v8::Undefined
#define V8_T_CONTEXT v8::Context
#define V8_T_NULL v8::Null

#define JS_V8_ARGUMENT v8::FunctionCallbackInfo<v8::Value>

// #public
#define V8_T_LOCAL(x) v8::Local<x>
#define V8_T_HANDLE(x) v8::Handle<x>
#define V8_T_PERSISTENT(x) v8::Persistent<x>

#define JS_UNDEFINED()                            \
  V8_T_LOCAL(V8_T_VALUE)::New(__contextORisolate, \
                              V8_T_UNDEFINED(__contextORisolate))
#define JS_NULL() \
  V8_T_LOCAL(V8_T_VALUE)::New(__contextORisolate, V8_T_NULL(__contextORisolate))

#define JS_NEW_ERROR_VALUE(x) v8::Exception::Error(x)
#define JS_NEW_OBJECT_TEMPLATE() V8_T_OBJECT_TEMPLATE::New(__contextORisolate)

#define JS_NEW_EMPTY_OBJECT() V8_T_OBJECT::New(__contextORisolate)
#define JS_NEW_EMPTY_STRING() V8_T_STRING::Empty(__contextORisolate)
#define _JS_NEW_INTEGER(x) V8_T_INTEGER::New(__contextORisolate, x)
#define _JS_NEW_UNSIGNED(x) V8_T_INTEGER::NewFromUnsigned(__contextORisolate, x)
#define _JS_NEW_NUMBER(x) V8_T_NUMBER::New(__contextORisolate, x)
#define _JS_NEW_BOOLEAN(x) V8_T_BOOLEAN::New(__contextORisolate, x)

#define JS_PERSISTENT_VALUE V8_T_PERSISTENT(V8_T_VALUE)
#define JS_PERSISTENT_VALUE_REF JS_PERSISTENT_VALUE
#define JS_PERSISTENT_OBJECT V8_T_PERSISTENT(V8_T_OBJECT)
#define JS_PERSISTENT_OBJECT_TEMPLATE V8_T_PERSISTENT(V8_T_OBJECT_TEMPLATE)
#define JS_PERSISTENT_ARRAY V8_T_PERSISTENT(V8_T_ARRAY)
#define JS_PERSISTENT_STRING V8_T_PERSISTENT(V8_T_STRING)
#define JS_PERSISTENT_FUNCTION V8_T_PERSISTENT(V8_T_FUNCTION)
#define JS_PERSISTENT_FUNCTION_TEMPLATE V8_T_PERSISTENT(V8_T_FUNCTION_TEMPLATE)
#define JS_PERSISTENT_CONTEXT V8_T_PERSISTENT(v8::Context)
#define JS_PERSISTENT_SCRIPT V8_T_PERSISTENT(V8_T_SCRIPT)

#define JS_HANDLE_CONTEXT V8_T_HANDLE(V8_T_CONTEXT)
#define JS_HANDLE_OBJECT V8_T_HANDLE(V8_T_OBJECT)
#define JS_HANDLE_OBJECT_REF JS_HANDLE_OBJECT
#define JS_HANDLE_OBJECT_TEMPLATE V8_T_HANDLE(V8_T_OBJECT_TEMPLATE)
#define JS_HANDLE_STRING V8_T_HANDLE(V8_T_STRING)
#define JS_HANDLE_STRING_CARRY JS_HANDLE_STRING
#define JS_HANDLE_VALUE V8_T_HANDLE(V8_T_VALUE)
#define JS_HANDLE_VALUE_REF JS_HANDLE_VALUE
#define JS_HANDLE_ARRAY V8_T_HANDLE(V8_T_ARRAY)
#define JS_HANDLE_INTEGER V8_T_HANDLE(V8_T_INTEGER)
#define JS_HANDLE_INTEGER_ V8_T_HANDLE(V8_T_INTEGER)()
#define JS_HANDLE_BOOLEAN V8_T_HANDLE(V8_T_BOOLEAN)
#define JS_HANDLE_FUNCTION V8_T_HANDLE(V8_T_FUNCTION)
#define JS_HANDLE_FUNCTION_REF JS_HANDLE_FUNCTION
#define JS_HANDLE_FUNCTION_TEMPLATE V8_T_HANDLE(V8_T_FUNCTION_TEMPLATE)
#define JS_HANDLE_SCRIPT V8_T_HANDLE(V8_T_SCRIPT)

#define JS_LOCAL_OBJECT V8_T_LOCAL(V8_T_OBJECT)
#define JS_LOCAL_OBJECT_TEMPLATE V8_T_LOCAL(V8_T_OBJECT_TEMPLATE)
#define JS_LOCAL_STRING V8_T_LOCAL(V8_T_STRING)
#define JS_LOCAL_SCRIPT V8_T_LOCAL(V8_T_SCRIPT)
#define JS_LOCAL_VALUE V8_T_LOCAL(V8_T_VALUE)
#define JS_LOCAL_ARRAY V8_T_LOCAL(V8_T_ARRAY)
#define JS_LOCAL_INTEGER V8_T_LOCAL(V8_T_INTEGER)
#define JS_LOCAL_BOOLEAN V8_T_LOCAL(V8_T_BOOLEAN)
#define JS_LOCAL_FUNCTION V8_T_LOCAL(V8_T_FUNCTION)
#define JS_LOCAL_FUNCTION_TEMPLATE V8_T_LOCAL(V8_T_FUNCTION_TEMPLATE)

#define TO_LOCAL_FUNCTION__(x) JS_LOCAL_FUNCTION::New(__contextORisolate, x)
#define TO_LOCAL_FUNCTION(x) TO_LOCAL_FUNCTION__(x)

#define JS_LOCAL_CONTEXT V8_T_LOCAL(V8_T_CONTEXT)

#define JS_OBJECT_FROM_PERSISTENT__(x) \
  JS_HANDLE_VALUE::New(__contextORisolate, x)
#define JS_OBJECT_FROM_PERSISTENT(x) JS_OBJECT_FROM_PERSISTENT__(x)->ToObject()

#define JS_VALUE_TO_OBJECT(x) x->ToObject()
#define JS_VALUE_TO_STRING(x) x->ToString()
#define JS_VALUE_TO_NUMBER(x) x->ToNumber()
#define JS_VALUE_TO_INTEGER(x) x->ToInteger()
#define JS_VALUE_TO_BOOLEAN(x) x->ToBoolean()
#define JS_STRING_TO_ERROR_VALUE(x) v8::Exception::Error(x)

#define INTEGER_TO_STD(x) (x)->IntegerValue()
#define INT32_TO_STD(x) (x)->Int32Value()
#define UINT32_TO_STD(x) (x)->Uint32Value()

#define STRING_TO_STD__(x) *node::Utf8Value(__contextORisolate, x)
#define STRING_TO_STD(x) STRING_TO_STD__(x)

#define BOOLEAN_TO_STD(x) (x)->BooleanValue()
#define NUMBER_TO_STD(x) (x)->NumberValue()

#define JS_HAS_NAME(x, y) (x)->Has(y)
#define JS_GET_INDEX(x, y) (x)->Get((uint32_t)y)
#define JS_GET_NAME(x, y) (x)->Get(y)
#define JS_GET_NAME_HIDDEN(x, y) (x)->GetHiddenValue(y)
#define JS_GET_FUNCTION(x) (x)->GetFunction()
#define JS_GET_CONSTRUCTOR(x) (x)->GetFunction()

#define JS_IS_EMPTY(x) (x).IsEmpty()
#define JS_IS_NULL_OR_UNDEFINED(x) ((x)->IsNull() || (x)->IsUndefined())
#define JS_IS_FUNCTION(x) (x)->IsFunction()
#define JS_IS_NUMBER(x) (x)->IsNumber()
#define JS_IS_BOOLEAN(x) (x)->IsBoolean()
#define JS_IS_STRING(x) (x)->IsString()
#define JS_IS_DATE(x) (x)->IsDate()
#define JS_IS_INT32(x) (x)->IsInt32()
#define JS_IS_UINT32(x) (x)->IsUint32()
#define JS_IS_TRUE(x) (x)->IsTrue()
#define JS_IS_FALSE(x) (x)->IsFalse()
#define JS_IS_REGEXP(x) (x)->IsRegExp()
#define JS_IS_NULL(x) (x)->IsNull()
#define JS_IS_UNDEFINED(x) (x)->IsUndefined()
#define JS_IS_OBJECT(x) (x)->IsObject()
#define JS_IS_ARRAY(x) (x)->IsArray()
#define JS_IS_NEARDEATH(x) (x)->IsNearDeath()
#define JS_IS_BUFFER(x) (x)->IsUint8Array()

#define BUFFER__DATA(x)                                            \
  (static_cast<char *>(                                            \
       (x).As<v8::Uint8Array>()->Buffer()->GetContents().Data()) + \
   ((x).As<v8::Uint8Array>()->ByteOffset()))

#define BUFFER__LENGTH(x) (x).As<v8::Uint8Array>()->ByteLength();

#define JS_NEW_ARRAY() V8_T_ARRAY::New(JS_GET_STATE_MARKER())

#define JS_NEW_ARRAY_WITH_COUNT__(n) V8_T_ARRAY::New(JS_GET_STATE_MARKER(), n)
#define JS_NEW_ARRAY_WITH_COUNT(n) JS_NEW_ARRAY_WITH_COUNT__(n)

#define JS_NEW_FUNCTION_TEMPLATE(x) \
  V8_T_FUNCTION_TEMPLATE::New(__contextORisolate, x)
#define JS_NEW_FUNCTION_CALL_TEMPLATE(x) \
  V8_T_FUNCTION_TEMPLATE::New(__contextORisolate, x)
#define JS_NEW_EMPTY_FUNCTION_TEMPLATE() \
  V8_T_FUNCTION_TEMPLATE::New(__contextORisolate)
#define JS_NEW_CONTEXT(x, a, b) v8::Local<v8::Context> x = V8_T_CONTEXT::New(a)

static inline v8::Local<v8::Context> __JS_NEW_EMPTY_CONTEXT() {
  return v8::Context::New(v8::Isolate::GetCurrent());
}
#define JS_NEW_EMPTY_CONTEXT() __JS_NEW_EMPTY_CONTEXT()

#define JS_NEW_PERSISTENT_CONTEXT(x, y) x.Reset(__contextORisolate, x)

#define JS_CAST_VALUE(x) JS_LOCAL_VALUE::Cast(x)
#define JS_CAST_OBJECT(x) JS_LOCAL_OBJECT::Cast(x)
#define JS_CAST_ARRAY(x) JS_LOCAL_ARRAY::Cast(x)
#define JS_CAST_STRING(x) JS_LOCAL_STRING::Cast(x)
#define JS_CAST_FUNCTION(x) JS_LOCAL_FUNCTION::Cast(x)
#define JS_CAST_FUNCTION_TEMPLATE(x) JS_LOCAL_FUNCTION_TEMPLATE::Cast(x)

#define JS_CLEAR_PERSISTENT_(x) x.Reset()

#define JS_CLEAR_PERSISTENT(x) \
  if (!x.IsEmpty()) {          \
    JS_CLEAR_PERSISTENT_(x);   \
  }

#define JS_NEW_PERSISTENT_OBJECT(a, x) a.Reset(__contextORisolate, x)
#define JS_NEW_PERSISTENT_OBJECT_TEMPLATE(x) \
  JS_PERSISTENT_OBJECT_TEMPLATE::New(__contextORisolate, x)
#define JS_NEW_PERSISTENT_ARRAY(x, y) x.Reset(__contextORisolate, y)
#define JS_NEW_EMPTY_PERSISTENT_OBJECT(a) \
  a.Reset(__contextORisolate, JS_NEW_EMPTY_OBJECT())
#define JS_NEW_PERSISTENT_STRING(a, b) a.Reset(__contextORisolate, b)
#define JS_NEW_PERSISTENT_FUNCTION(a, b) a.Reset(__contextORisolate, b)
#define JS_NEW_PERSISTENT_FUNCTION_TEMPLATE(a, b) a.Reset(__contextORisolate, b)
#define JS_NEW_EMPTY_PERSISTENT_FUNCTION_TEMPLATE(a) a.Reset()
#define JS_NEW_PERSISTENT_SCRIPT(a, x) a.Reset(__contextORisolate, x)
#define JS_DISPOSE_PERSISTENT_CONTEXT(x) (x).Reset()
#define JS_NEW_PERSISTENT_VALUE(a, x) a.Reset(__contextORisolate, x)

#define STD_TO_STRING(str) V8_T_STRING::NewFromUtf8(__contextORisolate, str)

#define UTF16_TO_STRING__(str) \
  V8_T_STRING::NewFromTwoByte(__contextORisolate, (const uint16_t *)str)

inline static JS_HANDLE_STRING ___STD_TO_STRING(v8::Isolate *__contextORisolate,
                                                const char *chr) {
  v8::EscapableHandleScope scope(__contextORisolate);
  return scope.Escape(STD_TO_STRING(chr));
}

inline static JS_HANDLE_STRING ___STD_TO_STRING(v8::Isolate *__contextORisolate,
                                                const uint16_t *chr) {
  v8::EscapableHandleScope scope(__contextORisolate);
  return scope.Escape(UTF16_TO_STRING__(chr));
}

#define UTF8_TO_STRING(str) ___STD_TO_STRING(__contextORisolate, str)

#define STD_TO_BOOLEAN(bl) _JS_NEW_BOOLEAN(bl)
#define STD_TO_INTEGER(nt) _JS_NEW_INTEGER(nt)
#define STD_TO_UNSIGNED(nt) _JS_NEW_UNSIGNED(nt)
#define STD_TO_NUMBER(n) _JS_NEW_NUMBER(n)

#define JS_RETAINED_OBJECT_INFO v8::RetainedObjectInfo

#define JS_TYPE_AS_FUNCTION(x) (x).As<v8::Function>()
#define JS_TYPE_AS_STRING(x) (x).As<v8::String>()
#define JS_TYPE_AS_OBJECT(x) (x).As<v8::Object>()
#define JS_TYPE_AS_ARRAY(x) (x).As<v8::Array>()

#define JS_TRY_CATCH(x) v8::TryCatch x(__contextORisolate)

#define JS_GET_STRING_LENGTH(a) (a)->Length()
#define JS_GET_ARRAY_LENGTH(a) (a)->Length()

#define JS_COMPARE_BY_CONSTRUCTOR(obj, cons) \
  obj->GetConstructor()->StrictEquals(cons)
#define JS_HAS_INSTANCE(obj, to_check) obj->HasInstance(to_check)

#define JS_STRICT_EQUALS(a, b) a->StrictEquals(b)

#define JS_METHOD_CALL(obj, method, argc, args) obj->Call(method, argc, args)
#define JS_METHOD_CALL_NO_PARAM(obj, method) obj->Call(method, 0, NULL)

#define JS_LOCAL_MESSAGE v8::Handle<v8::Message>

#define JS_COMPILE_AND_RUN__(a, b) v8::Script::Compile(a, b)->Run()
#define JS_COMPILE_AND_RUN(a, b) JS_COMPILE_AND_RUN__(a, b)

#define JS_SCRIPT_COMPILE(source, filename) \
  v8::Script::Compile(source, filename)
#define JS_SCRIPT_RUN(script) script->Run()

#define JS_NATIVE_RETURN_TYPE void

#define JS_TYPE_TO_LOCAL_SCRIPT(x) JS_LOCAL_SCRIPT::New(__contextORisolate, x)
#define JS_TYPE_TO_LOCAL_VALUE(x) JS_LOCAL_VALUE::New(__contextORisolate, x)
#define JS_TYPE_TO_LOCAL_OBJECT(x) JS_LOCAL_OBJECT::New(__contextORisolate, x)
#define JS_TYPE_TO_LOCAL_STRING(x) JS_LOCAL_STRING::New(__contextORisolate, x)
#define JS_TYPE_TO_LOCAL_FUNCTION(x) \
  JS_LOCAL_FUNCTION::New(__contextORisolate, x)
#define JS_TYPE_TO_LOCAL_FUNCTION_TEMPLATE(x) \
  JS_LOCAL_FUNCTION_TEMPLATE::New(__contextORisolate, x)
#define JS_TYPE_TO_LOCAL_CONTEXT(x) PersistentToLocal(__contextORisolate, x)
#define JS_NEW_LOCAL_CONTEXT(x) \
  v8::Local<v8::Context>::New(__contextORisolate, x)
#define JS_TYPE_TO_LOCAL_ARRAY(x) JS_LOCAL_ARRAY::New(__contextORisolate, x)

#define JS_MAKE_WEAK(p, c, t) p.SetWeak(c, t)

#define JS_WEAK_CALLBACK_METHOD(type, type2, name) \
  void name(const v8::WeakCallbackData<type, type2> &data)

#define JS_ENTER_SCOPE_WITH(x) v8::EscapableHandleScope scope(x)

#define JS_ENTER_SCOPE_COM()                                            \
  node::Environment *env = (node::Environment *)__GetNodeEnvironment(); \
  JS_ENTER_SCOPE_WITH(env->isolate())

#define JS_INDEX_SET(name, index, value) (name)->Set((uint32_t)index, value)
#define JS_NAME_SET(obj, name, value) (obj)->Set(name, value)
#define JS_NAME_SET_HIDDEN(obj, name, value) (obj)->SetHiddenValue(name, value)
#define JS_NAME_DELETE(obj, name) (obj)->Delete(name)

#define JS_HAS_NAME(x, y) (x)->Has(y)
#define JS_GET_INDEX(x, y) (x)->Get((uint32_t)y)
#define JS_GET_NAME(x, y) (x)->Get(y)
#define JS_GET_NAME_HIDDEN(x, y) (x)->GetHiddenValue(y)
#define JS_GET_FUNCTION(x) (x)->GetFunction()

#define JS_SET_POINTER_DATA(host, data) \
  host->SetAlignedPointerInInternalField(0, data)
#define JS_GET_POINTER_DATA(host) host->GetAlignedPointerFromInternalField(0)

#define JS_METHOD(method_name)                                \
  void method_name(const JS_V8_ARGUMENT &args) {              \
    v8::Isolate* __contextORisolate = args.GetIsolate();

#define JS_LOCAL_METHOD(method_name)                              \
  void method_name(const JS_V8_ARGUMENT &args) {                  \
    node::Environment *env = node::__GetNodeEnvironment();        \
    JS_DEFINE_STATE_MARKER(env);                                  \
    ScopeWatch watcher(__contextORisolate, true);

#define JS_THROW_EXCEPTION_TYPE(x) __contextORisolate->ThrowException(x)
#define JS_THROW_EXCEPTION(x) __contextORisolate->ThrowException(x)

#define THROW_EXCEPTION(msg)                                           \
  do {                                                                 \
    JS_LOCAL_VALUE p___err = v8::Exception::Error(STD_TO_STRING(msg)); \
    JS_THROW_EXCEPTION_TYPE(p___err);                                  \
    RETURN();                                                          \
  } while (0)

#define JS_METHOD_END }

#define RETURN() \
  do {           \
    return;      \
  } while (0)

#define RETURN_PARAM(param)           \
  do {                                \
    args.GetReturnValue().Set(param); \
    return;                           \
  } while (0)

#define JS_SCRIPT_COMPILE(source, filename) \
  v8::Script::Compile(source, filename)

#define JS_SCRIPT_RUN(script) script->Run()

#define JS_COMPILE_AND_RUN(a, b) v8::Script::Compile(a, b)->Run()

class JS_Method {
 public:
  JS_NATIVE_METHOD native_method_;
  bool is_native_method_;
  int interface_id_;

  JS_Method()
      : native_method_(0), is_native_method_(false), interface_id_(-1) {}
};

class JSFunctionWrapper {
  node::Environment *com_;
  JS_PERSISTENT_FUNCTION pst_fnc_;
  JS_PERSISTENT_OBJECT pst_obj_;

 public:
  JSFunctionWrapper(node::Environment *env, v8::Handle<v8::Function> fnc,
                    JS_HANDLE_VALUE obj) {
    com_ = env;
    JS_DEFINE_STATE_MARKER(env);
    JS_NEW_PERSISTENT_FUNCTION(pst_fnc_, fnc);
    if (!JS_IS_EMPTY(obj) && !JS_IS_UNDEFINED(obj)) {
      JS_NEW_PERSISTENT_OBJECT(pst_obj_, JS_VALUE_TO_OBJECT(obj));
    }
  }

  static JS_WEAK_CALLBACK_METHOD(v8::Object, void, dummyWeakCallbackObject) {}
  static JS_WEAK_CALLBACK_METHOD(v8::Function, void,
                                 dummyWeakCallbackFunction) {}
#define DWCBO_OBJ dummyWeakCallbackObject
#define DWCBO_FNC dummyWeakCallbackFunction

  void Dispose() {
    if (com_ == NULL) return;
    com_ = NULL;

    if (!JS_IS_EMPTY(pst_fnc_)) JS_MAKE_WEAK(pst_fnc_, (void *)NULL, DWCBO_FNC);

    if (!JS_IS_EMPTY(pst_obj_)) JS_MAKE_WEAK(pst_obj_, (void *)NULL, DWCBO_OBJ);
  }

  inline JS_LOCAL_FUNCTION GetFunction() {
    assert(com_ != NULL && "JSFunctionWrapper was already disposed.");
    JS_DEFINE_STATE_MARKER(com_);
    return JS_TYPE_TO_LOCAL_FUNCTION(pst_fnc_);
  }

  ~JSFunctionWrapper() { Dispose(); }

  JS_HANDLE_VALUE Call(const int argc, JS_HANDLE_VALUE args[], bool *success) {
    assert(com_ != NULL && "JSFunctionWrapper was already disposed.");

    JS_ENTER_SCOPE_WITH(com_->isolate());
    JS_DEFINE_STATE_MARKER(com_);
    node::Environment *env = com_;

    JS_TRY_CATCH(tc);
    JS_LOCAL_VALUE res;

    JS_LOCAL_FUNCTION l_fnc = JS_TYPE_TO_LOCAL_FUNCTION(pst_fnc_);
    if (!JS_IS_EMPTY(pst_obj_)) {
      JS_LOCAL_OBJECT l_obj = JS_TYPE_TO_LOCAL_OBJECT(pst_obj_);
      res = JS_METHOD_CALL(l_fnc, l_obj, argc, args);
    } else {
      JS_HANDLE_OBJECT glob = JS_GET_GLOBAL();
      res = JS_METHOD_CALL(l_fnc, glob, argc, args);
    }

    if (tc.HasCaught()) {
      *success = false;
      return JS_LEAVE_SCOPE(tc.Exception());
    }

    *success = true;
    return JS_LEAVE_SCOPE(res);
  }
};

class JSValueWrapper {
 public:
  JS_PERSISTENT_VALUE value_;

  JSValueWrapper() {}

  JSValueWrapper(node::Environment *env, JS_HANDLE_VALUE_REF value) {
    JS_DEFINE_STATE_MARKER(env);
    JS_NEW_PERSISTENT_VALUE(value_, value);
  }

  ~JSValueWrapper() {
    if (!JS_IS_EMPTY(value_)) {
      JS_CLEAR_PERSISTENT(value_);
    }
  }
};

#define _FREE_MEM_(x)                             \
  JSValueWrapper *wrap##x_ = (JSValueWrapper *)x; \
  delete wrap##x_

#define UNWRAP_COM(arg)                                   \
  assert(arg.com_ && "com_ can not be NULL");             \
  node::Environment *env = (node::Environment *)arg.com_; \
  JS_DEFINE_STATE_MARKER(env);                            \
  ScopeWatch watcher(__contextORisolate)

#define JS_ENGINE_LOCKER()                   \
  v8::Isolate *isolate = GET_ISOLATE(env);   \
  v8::Locker locker(isolate);                \
  v8::Isolate::Scope isolate_scope(isolate); \
  v8::HandleScope scope(isolate)

#define ENTER_ENGINE_SCOPE() \
  JS_ENGINE_LOCKER();        \
  v8::Context::Scope context_scope(env->context())

#define RUN_IN_SCOPE(x)            \
  if (watcher.EnteredNewScope()) { \
    ENTER_ENGINE_SCOPE();          \
    x                              \
  } else {                         \
    x                              \
  }

#define EMPTY_CHECK(x) \
  if (value.type_ == RT_Null || value.type_ == RT_Undefined) return x

#define UNWRAP_RESULT(x) JSValueWrapper *wrap = (JSValueWrapper *)x

namespace node {

void AddFile(const char* path, char *contents, size_t length);

Environment *__GetNodeEnvironment();
void __StartNodeInstance(void *arg, void (*DEFINE_NATIVES)());
bool __Loop(bool once);
void __Start(int argc, char **argv, void (*DEFINE_NATIVES)());
int __Shutdown();

}  // namespace node

#endif  // SRC_NODE_WRAPPER_H
