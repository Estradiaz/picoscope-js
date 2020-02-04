#include <napi.h>
#include <ps2000aApi.h>
#include "./PicoScope/ps_wrap.h"

Napi::Object Init(Napi::Env env, Napi::Object exports){
  PS::Init(env, exports);
  return exports;
}

NODE_API_MODULE(PicoScope, Init)