/*
JXcore Java bindings
The MIT License (MIT)

Copyright (c) 2015 Oguz Bastemur

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#define __STDC_LIMIT_MACROS
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include "jxcore_droid.h"
#include "JniHelper.h"
#include <android/log.h>
#include "android/asset_manager.h"
#include "android/asset_manager_jni.h"
#include <assert.h>
#include <map>

std::map<int, JS_Value*> store_map;
typedef std::map<int, JS_Value*>::iterator store_iter;
static JS_Value *cb_values = NULL;
static JS_Value eventCB;

static int store_counter = 0;

int JS_StoreValue(JS_Value *val) {
  // not thread safe, however we don't have multiple instance to interface
  int id = store_counter++;
  store_counter %= INT_MAX-1;

  JS_Value *bck = (JS_Value*) malloc(sizeof(JS_Value)); // hack! jxcore will be free'ing it
  memcpy(bck, val, sizeof(JS_Value));
  JS_MakePersistent(bck);
  store_map[id] = bck;
  return id;
}

JS_Value* JS_RemoveStoredValue(const int _, int id) {
  store_iter it = store_map.find(id);

  if (it != store_map.end()) {
    JS_Value *value = it->second;
    store_map.erase(it);
    JS_ClearPersistent(value);
    value->was_stored_ = true; // hack!! will be receving 'free' on JXcore end
    return value;
  } else {
    return NULL;
  }
}

int JS_GetStoredValueType(const int _, int id) {
  store_iter it = store_map.find(id);

  if (it != store_map.end()) {
    JS_Value *value = it->second;
    return (int)value->type_;
  } else {
    return -1;
  }
}

void ConvertResult(JS_Value *result, std::string &to_result) {
  switch (result->type_) {
    case RT_Null:
      to_result = "null";
      break;
    case RT_Undefined:
      to_result = "undefined";
      break;
    case RT_Boolean:
      to_result = JS_GetBoolean(result) ? "true" : "false";
      break;
    case RT_Int32: {
      std::stringstream ss;
      ss << JS_GetInt32(result);
      to_result = ss.str();
    } break;
    case RT_Double: {
      std::stringstream ss;
      ss << JS_GetDouble(result);
      to_result = ss.str();
    } break;
    case RT_Buffer:
    case RT_Object:
    case RT_String:
    case RT_Error: {
      char *chr = JS_GetString(result);
      to_result = chr;
      free(chr);
    } break;
    default:
      to_result = "null";
      return;
  }
}

static void callback(JS_Value *results, int argc) {
  if (argc != 3) {
    error_console(
        "wrong callback received. expected number of parameters was 3. "
        "received %d",
        argc);
    return;
  }

  if (!JS_IsString(results + 2)) {
    error_console(
        "JXcore-Cordova: Unexpected callback received. Third parameter must "
        "be a String");
    return;
  }

  cb_values = results;

  // results+1 -> err_val
  jxcore::Callback((JS_IsNull(results + 1) || JS_IsUndefined(results + 1)) ? 0
                                                                           : 1);

  cb_values = NULL;
}

static void callJXcoreNative(JS_Value *results, int argc) {
  JNIEnv *env = jxcore::JniHelper::getEnv();

  jclass arrClass = env->FindClass("java/util/ArrayList");
  jmethodID arrInit = env->GetMethodID(arrClass, "<init>", "()V");
  jobject objArray = env->NewObject(arrClass, arrInit);
  jmethodID addObject =
      env->GetMethodID(arrClass, "add", "(Ljava/lang/Object;)Z");

  jclass boolClass = NULL;
  jmethodID boolMethod;

  jclass intClass = NULL;
  jmethodID intMethod;

  jclass doubleClass = NULL;
  jmethodID doubleMethod;

  jclass strClass = env->FindClass("java/lang/String");

  for (int i = 0; i < argc; i++) {
    JS_Value *result = results + i;
    jobject objValue = NULL;

    switch (result->type_) {
      case RT_Boolean: {
        if (boolClass == NULL) {
          boolClass = env->FindClass("java/lang/Boolean");
          boolMethod = env->GetMethodID(boolClass, "<init>", "(Z)V");
        }
        jboolean bl = JS_GetBoolean(result) ? JNI_TRUE : JNI_FALSE;
        objValue = env->NewObject(boolClass, boolMethod, bl);
      } break;
      case RT_Int32: {
        if (intClass == NULL) {
          intClass = env->FindClass("java/lang/Integer");
          intMethod = env->GetMethodID(intClass, "<init>", "(I)V");
        }

        jint nt = JS_GetInt32(result);
        objValue = env->NewObject(intClass, intMethod, nt);
      } break;
      case RT_Double: {
        if (doubleClass == NULL) {
          doubleClass = env->FindClass("java/lang/Double");
          doubleMethod = env->GetMethodID(doubleClass, "<init>", "(D)V");
        }
        jdouble db = JS_GetDouble(result);
        objValue = env->NewObject(doubleClass, doubleMethod, db);
      } break;
      case RT_Buffer: {
        char *arr = JS_GetString(result);
        int length = JS_GetDataLength(result);

        jbyteArray ret = env->NewByteArray(length);
        env->SetByteArrayRegion(ret, 0, length, (jbyte *)arr);
        objValue = (jobject)ret;
        free(arr);
      } break;
      case RT_Object: {
        std::string str_result;
        ConvertResult(result, str_result);
        const char *data = str_result.c_str();
        int ln = JS_GetDataLength(result);
        if (ln > 0 && *data != '{' && *data != '[') {
          objValue = (jobject)env->NewStringUTF(str_result.c_str());
        } else {
          jobjectArray ret = (jobjectArray)env->NewObjectArray(
              1, strClass, env->NewStringUTF(""));

          jstring jstr = env->NewStringUTF(str_result.c_str());
          env->SetObjectArrayElement(ret, 0, jstr);

          objValue = (jobject)ret;
          env->DeleteLocalRef(jstr);
        }
      } break;
      case RT_Error:
      case RT_String: {
        std::string str_result;
        ConvertResult(result, str_result);

        objValue = env->NewStringUTF(str_result.c_str());
      } break;
      default:
        break;
    }

    env->CallBooleanMethod(objArray, addObject, objValue);
    env->DeleteLocalRef(objValue);
  }

  jxcore::CallJava(objArray);

  env->DeleteLocalRef(objArray);
  env->DeleteLocalRef(arrClass);
  env->DeleteLocalRef(strClass);

  if (doubleClass != NULL)
    env->DeleteLocalRef(doubleClass);

  if (intClass != NULL)
    env->DeleteLocalRef(intClass);

  if (boolClass != NULL)
    env->DeleteLocalRef(boolClass);
}

AAssetManager *assetManager;
static void assetExistsSync(JS_Value *results, int argc) {
  char *filename = JS_GetString(&results[0]);
  bool found = false;
  AAsset *asset =
      AAssetManager_open(assetManager, filename, AASSET_MODE_UNKNOWN);
  if (asset) {
    found = true;
    AAsset_close(asset);
  }

  JS_SetBoolean(&results[argc], found);
  free(filename);
}

static void assetReadSync(JS_Value *results, int argc) {
  char *filename = JS_GetString(&results[0]);

  AAsset *asset =
      AAssetManager_open(assetManager, filename, AASSET_MODE_UNKNOWN);

  free(filename);
  if (asset) {
    off_t fileSize = AAsset_getLength(asset);
    void *data = malloc(fileSize);
    int read_len = AAsset_read(asset, data, fileSize);

    JS_SetBuffer(&results[argc], (char *)data, read_len);
    free(data);
    AAsset_close(asset);
    return;
  }

  const char *err = "File doesn't exist";
  JS_SetError(&results[argc], err, strlen(err));
}

static void defineEventCB(JS_Value *results, int argc) {
  if (!JS_IsFunction(results + 1)) {
    error_console("defineEventCB expects a function");
    return;
  }

  JS_MakePersistent(results+1);
  eventCB = *(results + 1);
}

std::string files_json;
static void assetReadDirSync(JS_Value *results, int argc) {
  JS_SetJSON(&results[argc], files_json.c_str(), files_json.length());
}

extern "C" {

jint JNI_OnLoad(JavaVM *vm, void *reserved) { return jxcore::Initialize(vm); }

JNIEXPORT jlong JNICALL
Java_io_jxcore_node_jxcore_callCBString(JNIEnv *env, jobject thiz,
                                        jstring ev_name, jstring param,
                                        jint json) {
  if (eventCB.type_ == 0) {
    error_console("event callback is not ready yet.");
    return 0;
  }

  const char *str_ev = env->GetStringUTFChars(ev_name, 0);
  const char *str_param = env->GetStringUTFChars(param, 0);

  long ret_val = 0;
  JS_Value args[2];
  if (JS_CreateEmptyObject(&args[1])) {
    JS_Value JS_str_param;
    JS_New(&args[0]);
    JS_New(&JS_str_param);

    JS_SetString(&args[0], str_ev, 0);
    JS_SetString(&JS_str_param, str_param, 0);

    if (json != 1)
      JS_SetNamedProperty(&args[1], "str", &JS_str_param);
    else
      JS_SetNamedProperty(&args[1], "json", &JS_str_param);

    JS_Free(&JS_str_param);

    JS_Value out;
    JS_CallFunction(&eventCB, args, 2, &out);

    JS_Free(&args[0]);
    JS_Free(&args[1]);

    if (!JS_IsNull(&out) && !JS_IsUndefined(&out))
      ret_val = JS_StoreValue(&out);
    else
      ret_val = -1;
  } else {
    error_console("couldn't create JS_Value Object");
  }

  env->ReleaseStringUTFChars(ev_name, str_ev);
  env->ReleaseStringUTFChars(param, str_param);

  return ret_val;
}

JNIEXPORT jlong JNICALL
Java_io_jxcore_node_jxcore_callCBArray(JNIEnv *env, jobject thiz,
                                       jstring ev_name, jobjectArray params,
                                       jint size) {
  if (eventCB.type_ == 0) {
    error_console("event callback is not ready yet.");
    return 0;
  }

  const char *str_ev = env->GetStringUTFChars(ev_name, 0);

  long ret_val = 0;
  JS_Value args[2];
  if (JS_CreateArrayObject(&args[1])) {
    JS_New(&args[0]);

    JS_SetString(&args[0], str_ev, 0);

    jclass boolClass = env->FindClass("java/lang/Boolean");
    jclass doubleClass = env->FindClass("java/lang/Double");
    jclass intClass = env->FindClass("java/lang/Integer");
    jclass strClass = env->FindClass("java/lang/String");
    jclass barrClass = env->FindClass("[B");

    for (int i = 0; i < (int)size; i++) {
      jobject elm = (jobject)env->GetObjectArrayElement(params, i);

      JS_Value val;
      JS_New(&val);
      if (elm == NULL) {
        JS_SetNull(&val);
      } else if (env->IsInstanceOf(elm, boolClass) == JNI_TRUE) {
        jmethodID bvalID = env->GetMethodID(boolClass, "booleanValue", "()Z");
        bool nval = (bool)env->CallBooleanMethod(elm, bvalID);
        JS_SetBoolean(&val, nval);
      } else if (env->IsInstanceOf(elm, intClass) == JNI_TRUE) {
        jmethodID bvalID = env->GetMethodID(intClass, "intValue", "()I");
        int nval = (int)env->CallIntMethod(elm, bvalID);
        JS_SetInt32(&val, nval);
      } else if (env->IsInstanceOf(elm, doubleClass) == JNI_TRUE) {
        jmethodID bvalID = env->GetMethodID(doubleClass, "doubleValue", "()D");
        double nval = (double)env->CallDoubleMethod(elm, bvalID);
        JS_SetDouble(&val, nval);
      } else if (env->IsInstanceOf(elm, strClass) == JNI_TRUE) {
        jstring jstr = (jstring)elm;
        const char *str = env->GetStringUTFChars(jstr, 0);
        JS_SetString(&val, str, strlen(str));
        env->ReleaseStringUTFChars(jstr, str);
      } else if (env->IsInstanceOf(elm, barrClass) == JNI_TRUE) {
        jbyteArray jarr = (jbyteArray)elm;
        int len = env->GetArrayLength(jarr);
        jbyte *barr = env->GetByteArrayElements(jarr, 0);
        JS_SetBuffer(&val, (char *)barr, len);

        env->ReleaseByteArrayElements(jarr, barr, JNI_ABORT);
      }

      JS_SetIndexedProperty(&args[1], i, &val);
      JS_Free(&val);

      env->DeleteLocalRef(elm);
    }

    env->DeleteLocalRef(boolClass);
    env->DeleteLocalRef(doubleClass);
    env->DeleteLocalRef(intClass);
    env->DeleteLocalRef(strClass);
    env->DeleteLocalRef(barrClass);

    JS_Value out;
    JS_CallFunction(&eventCB, args, 2, &out);

    JS_Free(&args[0]);
    JS_Free(&args[1]);

    if (!JS_IsNull(&out) && !JS_IsUndefined(&out))
      ret_val = JS_StoreValue(&out);
    else
      ret_val = -1;
  } else {
    error_console("couldn't create JS_Value Object");
  }

  env->ReleaseStringUTFChars(ev_name, str_ev);

  return ret_val;
}

char *hfolder;

JNIEXPORT void JNICALL
Java_io_jxcore_node_jxcore_prepareEngine(JNIEnv *env, jobject thiz,
                                         jstring home, jstring files) {
  static bool initialized = false;

  const char *hfiles = env->GetStringUTFChars(files, 0);
  hfolder = strdup(env->GetStringUTFChars(home, 0));

  files_json = hfiles;

  env->ReleaseStringUTFChars(home, hfolder);
  env->ReleaseStringUTFChars(files, hfiles);

  JS_SetProcessNative("assetExistsSync", assetExistsSync);
  JS_SetProcessNative("assetReadSync", assetReadSync);
  JS_SetProcessNative("assetReadDirSync", assetReadDirSync);
  JS_SetProcessNative("defineEventCB", defineEventCB);
  JS_SetProcessNative("callJXcoreNative", callJXcoreNative);
}

JNIEXPORT jlong JNICALL
Java_io_jxcore_node_jxcore_evalEngine(JNIEnv *env, jobject thiz,
                                      jstring contents) {
  const char *data = env->GetStringUTFChars(contents, 0);

  JS_Value result;
  JS_Evaluate(data, 0, &result);

  env->ReleaseStringUTFChars(contents, data);

  if (!JS_IsNull(&result) && !JS_IsUndefined(&result))
    return JS_StoreValue(&result);
  else
    return -1;
}

JNIEXPORT jstring JNICALL
Java_io_jxcore_node_jxcore_convertToString(JNIEnv *env, jobject thiz,
                                           jlong id) {
  JS_Value *val;
  if (id < 0)
    val = cb_values + (id + 3);
  else
    val = JS_RemoveStoredValue(0, id);

  std::string str_result;
  ConvertResult(val, str_result);
  if (id >= 0) JS_Free(val);

  return env->NewStringUTF(str_result.c_str());
}

JNIEXPORT jint JNICALL
Java_io_jxcore_node_jxcore_getType(JNIEnv *env, jobject thiz, jlong id) {
  if (id < 0 && cb_values != NULL) return cb_values[id + 3].type_;

  if (id < 0) return RT_Undefined;

  return JS_GetStoredValueType(0, id);
}

JNIEXPORT jint JNICALL
Java_io_jxcore_node_jxcore_getInt32(JNIEnv *env, jobject thiz, jlong id) {
  if (id < 0) return JS_GetInt32(cb_values + (id + 3));

  JS_Value *val = JS_RemoveStoredValue(0, id);
  int n = JS_GetInt32(val);
  JS_Free(val);
  return n;
}

JNIEXPORT jdouble JNICALL
Java_io_jxcore_node_jxcore_getDouble(JNIEnv *env, jobject thiz, jlong id) {
  if (id < 0) return JS_GetDouble(cb_values + (id + 3));

  JS_Value *val = JS_RemoveStoredValue(0, id);
  int n = JS_GetDouble(val);
  JS_Free(val);
  return n;
}

JNIEXPORT jint JNICALL
Java_io_jxcore_node_jxcore_getBoolean(JNIEnv *env, jobject thiz, jlong id) {
  if (id < 0) return JS_GetBoolean(cb_values + (id + 3)) ? 1 : 0;

  JS_Value *val = JS_RemoveStoredValue(0, id);
  int n = JS_GetBoolean(val) ? 1 : 0;
  JS_Free(val);
  return n;
}

JNIEXPORT jstring JNICALL
Java_io_jxcore_node_jxcore_getString(JNIEnv *env, jobject thiz, jlong id) {
  JS_Value *val;
  if (id < 0)
    val = cb_values + (id + 3);
  else
    val = JS_RemoveStoredValue(0, id);

  char *chrp = JS_GetString(val);
  if (id >= 0) JS_Free(val);

  jstring js = env->NewStringUTF(chrp);
  free(chrp);

  return js;
}

JNIEXPORT jbyteArray JNICALL
Java_io_jxcore_node_jxcore_getBuffer(JNIEnv *env, jobject thiz, jlong id) {
  JS_Value *val;
  if (id < 0)
    val = cb_values + (id + 3);
  else
    val = JS_RemoveStoredValue(0, id);

  char *arr = JS_GetString(val);
  int length = JS_GetDataLength(val);

  jbyteArray ret = env->NewByteArray(length);
  env->SetByteArrayRegion(ret, 0, length, (jbyte *)arr);
  free(arr);
  if (id >= 0) JS_Free(val);
  return ret;
}

JNIEXPORT void JNICALL
Java_io_jxcore_node_jxcore_defineMainFile(JNIEnv *env, jobject obj,
                                          jstring contents) {
  const char *data = env->GetStringUTFChars(contents, 0);
  JS_DefineMainFile(data);
  env->ReleaseStringUTFChars(contents, data);
}

JNIEXPORT void JNICALL
Java_io_jxcore_node_jxcore_defineFile(JNIEnv *env, jobject obj,
                                      jstring filename, jstring data) {
  const char *name = env->GetStringUTFChars(filename, 0);
  const char *file = env->GetStringUTFChars(data, 0);

  JS_DefineFile(name, file);

  env->ReleaseStringUTFChars(filename, name);
  env->ReleaseStringUTFChars(data, file);
}

JNIEXPORT void JNICALL
Java_io_jxcore_node_jxcore_startEngine(JNIEnv *env, jobject thiz) {
  JS_StartEngine(hfolder);
}

JNIEXPORT jint JNICALL
Java_io_jxcore_node_jxcore_loopOnce(JNIEnv *env, jobject thiz) {
  return JS_LoopOnce();
}

JNIEXPORT void JNICALL
Java_io_jxcore_node_jxcore_stopEngine(JNIEnv *env, jobject thiz) {
  JS_ClearPersistent(&eventCB);
  JS_Free(&eventCB);
  JS_StopEngine();
}

JNIEXPORT void JNICALL
Java_io_jxcore_node_jxcore_setNativeContext(JNIEnv *env, jobject thiz,
                                            jobject context,
                                            jobject j_assetManager) {
  jxcore::JniHelper::setClassLoaderFrom(context);
  assetManager = AAssetManager_fromJava(env, j_assetManager);
}
}
