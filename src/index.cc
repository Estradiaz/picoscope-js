#include <napi.h>
#include <ps2000aApi.h>
#include "./PicoScope/PicoScope.h"

Napi::Object Init(Napi::Env env, Napi::Object exports){
  PicoScope::Init(env, exports);
  return exports;
}

NODE_API_MODULE(PicoScope, Init)