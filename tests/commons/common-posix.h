// See LICENSE file

#ifndef COMMON_POSIX_H_
#define COMMON_POSIX_H_

#ifndef _WIN32
#include <unistd.h>
#include <pthread.h>
#include "node_wrapper.h"
#else
#include <Windows.h>
#define usleep Sleep
#include "../../patch/node/node_wrapper.h"
#endif
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <assert.h>

#define flush_console(...)        \
  do {                            \
    fprintf(stdout, __VA_ARGS__); \
    fflush(stdout);               \
  } while (0)

void ConvertResult(JS_Value &result, std::string &to_result) {
  switch (result.type_) {
    case JS_ValueType::RT_Null:
      to_result = "null";
      break;
    case JS_ValueType::RT_Undefined:
      to_result = "undefined";
      break;
    case JS_ValueType::RT_Boolean:
      to_result = JS_GetBoolean(&result) ? "true" : "false";
      break;
    case JS_ValueType::RT_Int32: {
      std::stringstream ss;
      ss << JS_GetInt32(&result);
      to_result = ss.str();
    } break;
    case JS_ValueType::RT_Double: {
      std::stringstream ss;
      ss << JS_GetDouble(&result);
      to_result = ss.str();
    } break;
    case JS_ValueType::RT_Buffer:
    case JS_ValueType::RT_Object:
    case JS_ValueType::RT_Error:
    case JS_ValueType::RT_String: {
      char *_ = JS_GetString(&result);
      to_result = _;
      free(_);
    } break;
    default:
      to_result = "null";
      return;
  }
}

#endif /* COMMON_POSIX_H_ */
