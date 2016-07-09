#include "node.h"
#include "node_internals.h"
#include "node_watchdog.h"
#include "base-object.h"
#include "base-object-inl.h"
#include "env.h"
#include "env-inl.h"
#include "util.h"
#include "util-inl.h"
#include "v8-debug.h"
#include <map>
#include "node_internal_wrapper.h"
#include <string>

namespace node {

using v8::Array;
using v8::ArrayBuffer;
using v8::Boolean;
using v8::Context;
using v8::Debug;
using v8::EscapableHandleScope;
using v8::External;
using v8::Function;
using v8::FunctionCallbackInfo;
using v8::FunctionTemplate;
using v8::HandleScope;
using v8::Integer;
using v8::Isolate;
using v8::Local;
using v8::Maybe;
using v8::MaybeLocal;
using v8::Name;
using v8::NamedPropertyHandlerConfiguration;
using v8::Object;
using v8::ObjectTemplate;
using v8::Persistent;
using v8::PropertyAttribute;
using v8::PropertyCallbackInfo;
using v8::Script;
using v8::ScriptCompiler;
using v8::ScriptOrigin;
using v8::String;
using v8::TryCatch;
using v8::Uint8Array;
using v8::UnboundScript;
using v8::V8;
using v8::Value;
using v8::WeakCallbackInfo;

// VFile is NOT ThreadSafe!!!!
// And at the moment there is no need for it
struct VFile {
  static size_t inode_counter;
  size_t ino;
  size_t length;
  char* contents;
  
  VFile(char* contents_, size_t length_):ino(++inode_counter),
                    length(length_), contents(contents_) { };
};

size_t VFile::inode_counter = 0;
static std::map<std::string, VFile*> files;
typedef std::map<std::string, VFile*>::iterator VFIter;

void AddFile(const char* path, char *contents, size_t length) {
  VFile *vf = new VFile(contents, length);
  files[path] = vf;

  std::string str = "../";
  str += path;
  files[str.c_str()] = vf;
}

VFile* GetFile(std::string &path) {
  VFIter iter = files.find(path);
  if (iter != files.end()) {
    return iter->second;
  }
  return nullptr;
}

class NodeWrapVFile {
 public:
  static JS_METHOD(GetFileData) {
    if (args.Length() < 1 || !JS_IS_STRING(args[0])) {
      // fail silently
      RETURN();     
    }

    std::string path = STRING_TO_STD(args[0]);
    VFile* vf = GetFile(path);

    if (vf) {
      JS_LOCAL_OBJECT obj = JS_NEW_EMPTY_OBJECT();
      JS_NAME_SET(obj, STD_TO_STRING("ino"), STD_TO_NUMBER(vf->ino));
      JS_NAME_SET(obj, STD_TO_STRING("length"), STD_TO_NUMBER(vf->length));
      if (args.Length() == 2 && JS_IS_BOOLEAN(args[1]) && BOOLEAN_TO_STD(args[1])) {
        JS_NAME_SET(obj, STD_TO_STRING("contents"), UTF8_TO_STRING(vf->contents));
      }
      RETURN_PARAM(obj);
    }
  }
  JS_METHOD_END

  static void Init(Environment* env, Local<Object> target) {
    // Local<FunctionTemplate> function_template =
    //     FunctionTemplate::New(env->isolate());
    // function_template->InstanceTemplate()->SetInternalFieldCount(1);
    // env->set_script_data_constructor_function(function_template->GetFunction());

    env->SetMethod(target, "getFileData", GetFileData);
  }
};

void VFileInitialize(Local<Object> target,
                    Local<Value> unused,
                    Local<Context> context) {
  Environment* env = Environment::GetCurrent(context);
  NodeWrapVFile::Init(env, target);
}

}  // namespace node

NODE_MODULE_CONTEXT_AWARE_BUILTIN(node_wrap_vfile, node::VFileInitialize)
