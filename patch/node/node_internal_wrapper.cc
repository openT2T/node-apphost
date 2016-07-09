// See LICENSE file

static NodeInstanceData* instance_data;
static int __exec_argc;
static const char** __exec_argv;
static Environment* node_env;
Isolate::CreateParams isolate_params;
ArrayBufferAllocator array_buffer_allocator;
IsolateData* node_isolate_data;

Environment* __GetNodeEnvironment() { return node_env; }

void __StartNodeInstance(void* arg, void (*DEFINE_NATIVES)()) {
  isolate_params.array_buffer_allocator = &array_buffer_allocator;
#ifdef NODE_ENABLE_VTUNE_PROFILING
  isolate_params.code_event_handler = vTune::GetVtuneCodeEventHandler();
#endif
  Isolate* isolate = Isolate::New(isolate_params);

  {
    Mutex::ScopedLock scoped_lock(node_isolate_mutex);
    if (instance_data->is_main()) {
      CHECK_EQ(node_isolate, nullptr);
      node_isolate = isolate;
    }
  }

  if (track_heap_objects) {
    isolate->GetHeapProfiler()->StartTrackingHeapObjects(true);
  }

  {
    Locker locker(isolate);
    Isolate::Scope isolate_scope(isolate);
    HandleScope handle_scope(isolate);
    node_isolate_data =
        new IsolateData(isolate, instance_data->event_loop(),
                        array_buffer_allocator.zero_fill_field());
    Local<Context> context = Context::New(isolate);
    Context::Scope context_scope(context);
    node_env = new Environment(node_isolate_data, context);
    node_env->Start(instance_data->argc(), instance_data->argv(),
                    instance_data->exec_argc(), instance_data->exec_argv(),
                    v8_is_profiling);

    isolate->SetAbortOnUncaughtExceptionCallback(
        ShouldAbortOnUncaughtException);

    // Start debug agent when argv has --debug
    if (instance_data->use_debug_agent())
      StartDebug(node_env, debug_wait_connect);
      
    DEFINE_NATIVES();

    {
      Environment::AsyncCallbackScope callback_scope(node_env);
      LoadEnvironment(node_env);
    }

    node_env->set_trace_sync_io(trace_sync_io);

    // Enable debugger
    if (instance_data->use_debug_agent()) EnableDebug(node_env);
  }
}

bool __Loop(bool once) {
  v8::Isolate* isolate = node_isolate;
  Locker locker(isolate);
  Isolate::Scope isolate_scope(isolate);
  HandleScope handle_scope(isolate);

  bool more;
  {
    SealHandleScope seal(isolate);
    do {
      v8_platform.PumpMessageLoop(isolate);
      more = uv_run(node_env->event_loop(), UV_RUN_ONCE);

      if (more == false) {
        v8_platform.PumpMessageLoop(isolate);
        EmitBeforeExit(node_env);

        // Emit `beforeExit` if the loop became alive either after emitting
        // event, or after running some callbacks.
        more = uv_loop_alive(node_env->event_loop());
        if (uv_run(node_env->event_loop(), UV_RUN_NOWAIT) != 0) more = true;
      }
    } while (!once && more == true);
  }

  return more;
}

void __Start(int argc, char** argv, void (*DEFINE_NATIVES)()) {
  PlatformInit();

  CHECK_GT(argc, 0);

  // Hack around with the argv pointer. Used for process.title = "blah".
  argv = uv_setup_args(argc, argv);

  Init(&argc, const_cast<const char**>(argv), &__exec_argc, &__exec_argv);

#if HAVE_OPENSSL
#ifdef NODE_FIPS_MODE
  // In the case of FIPS builds we should make sure
  // the random source is properly initialized first.
  OPENSSL_init();
#endif  // NODE_FIPS_MODE
  // V8 on Windows doesn't have a good source of entropy. Seed it from
  // OpenSSL's pool.
  V8::SetEntropySource(crypto::EntropySource);
#endif

  v8_platform.Initialize(v8_thread_pool_size);
  V8::Initialize();

  instance_data =
      new NodeInstanceData(NodeInstanceType::MAIN, uv_default_loop(), argc,
                           const_cast<const char**>(argv), __exec_argc,
                           __exec_argv, use_debug_agent);

  __StartNodeInstance(instance_data, DEFINE_NATIVES);
}

int __Shutdown() {
  v8::Isolate* isolate = node_isolate;
  {
    Locker locker(isolate);
    Isolate::Scope isolate_scope(isolate);
    HandleScope handle_scope(isolate);

    node_env->set_trace_sync_io(false);

    int exit_code = EmitExit(node_env);
    if (instance_data->is_main()) instance_data->set_exit_code(exit_code);
    RunAtExit(node_env);

    WaitForInspectorDisconnect(node_env);
#if defined(LEAK_SANITIZER)
    __lsan_do_leak_check();
#endif
    delete node_env;
    delete node_isolate_data;
  }

  {
    Mutex::ScopedLock scoped_lock(node_isolate_mutex);
    if (node_isolate == isolate) node_isolate = nullptr;
  }

  CHECK_NE(isolate, nullptr);
  isolate->Dispose();
  isolate = nullptr;

  int exit_code = 1;
  exit_code = instance_data->exit_code();
  delete instance_data;

  V8::Dispose();

  v8_platform.Dispose();

  delete[] __exec_argv;
  __exec_argv = nullptr;

  return exit_code;
}
