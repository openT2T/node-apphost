// See LICENSE file

#ifndef COMMON_POSIX_H_
#define COMMON_POSIX_H_

#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>
#include "node_wrapper.h"


#define flush_console(...)        \
  do {                            \
    fprintf(stdout, __VA_ARGS__); \
    fflush(stdout);               \
  } while (0)

void ConvertResult(JS_Value &result, std::string &to_result) {
  switch (result.type_) {
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
    case RT_Error:
    case RT_String: {
      char *_ = JS_GetString(result);
      to_result = _;
      free(_);
    } break;
    default:
      to_result = "null";
      return;
  }
}

#endif /* COMMON_POSIX_H_ */
