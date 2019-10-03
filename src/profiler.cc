#include "node.h"
#include "nan.h"
#include "cpu_profiler.h"

namespace nodex {
  void InitializeProfiler(v8::Local<v8::Object> target) {
    Nan::HandleScope scope;
    CpuProfiler::Initialize(target);
  }

  NODE_MODULE(profiler, InitializeProfiler)
}
