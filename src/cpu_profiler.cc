#include "cpu_profiler.h"
#include "cpu_profile.h"

namespace nodex {
  using v8::CpuProfile;
  using v8::Local;
  using v8::Object;
  using v8::Array;
  using v8::String;

#if (NODE_MODULE_VERSION >= 69)
  static v8::CpuProfiler *_p = nullptr;
#endif

  CpuProfiler::CpuProfiler () {}
  CpuProfiler::~CpuProfiler () {
#if (NODE_MODULE_VERSION >= 69)
    if (_p != nullptr) _p->Dispose();
#endif
  }

  void CpuProfiler::Initialize (Local<Object> target) {
    Nan::HandleScope scope;

    Local<Object> cpuProfiler = Nan::New<Object>();
    Local<Array> profiles = Nan::New<Array>();

    Nan::SetMethod(cpuProfiler, "startProfiling", CpuProfiler::StartProfiling);
    Nan::SetMethod(cpuProfiler, "stopProfiling", CpuProfiler::StopProfiling);
    Nan::SetMethod(cpuProfiler, "setSamplingInterval", CpuProfiler::SetSamplingInterval);
    cpuProfiler->Set(Nan::New<String>("profiles").ToLocalChecked(), profiles);

    Profile::profiles.Reset(profiles);
    target->Set(Nan::New<String>("cpu").ToLocalChecked(), cpuProfiler);
  }

  NAN_METHOD(CpuProfiler::StartProfiling) {
    Local<String> title = info[0]->ToString();

#if (NODE_MODULE_VERSION >= 69)
    if (_p == nullptr) {
      _p = v8::CpuProfiler::New(v8::Isolate::GetCurrent());
    }

    bool recsamples = info[1]->ToBoolean()->Value();

    if (_p != nullptr) {
      _p->StartProfiling(title, recsamples);
    }
#elif (NODE_MODULE_VERSION > 0x000B)
    bool recsamples = info[1]->ToBoolean()->Value();
    v8::Isolate::GetCurrent()->GetCpuProfiler()->StartProfiling(title, recsamples);
#else
    v8::CpuProfiler::StartProfiling(title);
#endif
  }

  NAN_METHOD(CpuProfiler::StopProfiling) {
    const CpuProfile* profile = nullptr;

    Local<String> title = Nan::EmptyString();
    if (info.Length()) {
      if (info[0]->IsString()) {
        title = info[0]->ToString();
      } else if (!info[0]->IsUndefined()) {
        return Nan::ThrowTypeError("Wrong argument [0] type (wait String)");
      }
    }

#if (NODE_MODULE_VERSION >= 69)
    if (_p != nullptr) {
      profile = _p->StopProfiling(title);

      //TODO: Calling this will crash the app, fix possible memory leak.
      //_p->Dispose();
      //_p = nullptr;
    }
#elif (NODE_MODULE_VERSION > 0x000B)
    profile = v8::Isolate::GetCurrent()->GetCpuProfiler()->StopProfiling(title);
#else
    profile = v8::CpuProfiler::StopProfiling(title);
#endif

    info.GetReturnValue().Set(Profile::New(profile));
  }

  NAN_METHOD(CpuProfiler::SetSamplingInterval) {
#if (NODE_MODULE_VERSION >= 69)
    if (_p != nullptr) {
      _p->SetSamplingInterval(info[0]->Uint32Value());
    }
#elif (NODE_MODULE_VERSION > 0x000B)
    v8::Isolate::GetCurrent()->GetCpuProfiler()->SetSamplingInterval(info[0]->Uint32Value());
#endif
  }
} //namespace nodex
