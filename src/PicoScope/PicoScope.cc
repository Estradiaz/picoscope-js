#include "./PicoScope.h"
#include <ps2000.h>
#include "./ConnectWorker/ConnectWorker.h"
#include "./StreamWorker/StreamWorker.h"


// const uint32_t PicoScope::OVERVIEW_BUFFER_SIZE = 20000;

Napi::FunctionReference PicoScope::constructor;

Napi::Object PicoScope::Init(Napi::Env env, Napi::Object exports){
    Napi::HandleScope scope(env);

    Napi::Function func = DefineClass(
        env, 
        "PS", 
        {
            InstanceMethod("connect", &PicoScope::connect),
            InstanceMethod("disconnect", &PicoScope::disconnect),
            InstanceMethod("setChannel", &PicoScope::setChannel),
            InstanceMethod("getChannel", &PicoScope::getChannel),
            InstanceMethod("setTrigger", &PicoScope::setTrigger),
            InstanceMethod("stream", &PicoScope::stream),
            InstanceMethod("stop", &PicoScope::stop),
            // InstanceMethod("templateIndex", &PicoScope::getTemplateIndex)
        }
    );
    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();

    exports.Set("PicoScope", func);
    return exports;
}

Napi::Value PicoScope::connect(const Napi::CallbackInfo &info){

    if(info.Length() == 1 && info[0].IsFunction()){
        Napi::Function callback = info[0].As<Napi::Function>();
        ConnectWorker *worker = new ConnectWorker(callback, &this->deviceHandle, &this->connectState);
        worker->Queue();
        return info.Env().Undefined();
    }
    this->deviceHandle = ps2000_open_unit();
    if(deviceHandle != 0){
        connectState |= 1;
    }
    return Napi::Number::New(info.Env(), deviceHandle);
}

bool PicoScope::connected(){
    return connectState & 1;
}

bool PicoScope::connecting(){
    return connectState & 2;
}

Napi::Value PicoScope::getChannel(const Napi::CallbackInfo &info){

    if(info.Length() < 1){
        Napi::Error().New(info.Env(), "args missing < 1").ThrowAsJavaScriptException();
        return Napi::Number::New(info.Env(), -1);
    } 
    if(info[0].IsNumber()){

        int16_t channel = (uint32_t)info[0].As<Napi::Number>();
        Napi::Object obj = Napi::Object::New(info.Env());
        if(channel == 0){

            obj.Set("name", Napi::String::New(info.Env(), "A"));
            obj.Set("enabled", Napi::Number::New(info.Env(), A.enabled));
            obj.Set("dc", Napi::Number::New(info.Env(), A.dc));
            obj.Set("range", Napi::Number::New(info.Env(), A.range));
            obj.Set("direction", Napi::Number::New(info.Env(), A.direction));

            return obj;
        } else if(channel == 1){

            obj.Set("name", Napi::String::New(info.Env(), "B"));
            obj.Set("enabled", Napi::Number::New(info.Env(), B.enabled));
            obj.Set("dc", Napi::Number::New(info.Env(), B.dc));
            obj.Set("range", Napi::Number::New(info.Env(), B.range));
            obj.Set("direction", Napi::Number::New(info.Env(), B.direction));

            return obj;
        }
    }
}

Napi::Value PicoScope::setChannel(const Napi::CallbackInfo &info){

    if(TEST) printf("\n\n\n\n\nconnectState%d\n\n\n\n\n", connectState);
    if(!this->connected()){
        Napi::Error().New(info.Env(), "Connect to device first").ThrowAsJavaScriptException();
        return Napi::Number::New(info.Env(), -1);
    }
    // channel, active
    if(info.Length() < 4){
        Napi::Error().New(info.Env(), "args missing < 4").ThrowAsJavaScriptException();
        return Napi::Number::New(info.Env(), -1);
    } 
    if(!info[0].IsNumber() || (((uint32_t)info[0].As<Napi::Number>() != 0) && ((uint32_t)info[0].As<Napi::Number>() != 1))){
        Napi::Error().New(info.Env(), "first arg has to be number -> channel A (0), B (1)").ThrowAsJavaScriptException();
        return Napi::Number::New(info.Env(), -1);
    }
    if(!info[1].IsNumber() || ((uint32_t)info[1].As<Napi::Number>() != 0 && (uint32_t)info[1].As<Napi::Number>() != 1)){
        Napi::Error().New(info.Env(), "second arg has to be number -> inactive (0), active (1)").ThrowAsJavaScriptException();
        return Napi::Number::New(info.Env(), -1);
    }
    if(!info[2].IsNumber() || ((uint32_t)info[2].As<Napi::Number>() != 0 && (uint32_t)info[2].As<Napi::Number>() != 1)){
        Napi::Error().New(info.Env(), "third arg has to be number -> AC coupling (0), DC coupling (1)").ThrowAsJavaScriptException();
        return Napi::Number::New(info.Env(), -1);
    }
    if(!info[3].IsNumber() || ((uint32_t)info[3].As<Napi::Number>() <= 0 && (uint32_t)info[3].As<Napi::Number>() > 10)){
        Napi::Error().New(info.Env(), "third arg has to be number -> range +-20mV (1), +-20 V (10)").ThrowAsJavaScriptException();
        return Napi::Number::New(info.Env(), -1);
    }

    int hresult = -1;
    uint16_t channel = (uint32_t)info[0].As<Napi::Number>();
    int16_t enabled = (uint32_t)info[1].As<Napi::Number>();
    int16_t dc = (uint32_t)info[2].As<Napi::Number>();
    int16_t range = (uint32_t)info[3].As<Napi::Number>();
    hresult = ps2000_set_channel(deviceHandle, channel, enabled, dc, range);
    if(hresult != 0){

        if(channel == CHANNEL_A){
            A.enabled = enabled;
            A.range = range;
            A.dc = dc;
        } else if(channel == CHANNEL_B){
            B.enabled = enabled;
            B.range = range;
            B.dc = dc;
        }
    } else {
        Napi::Error::New(info.Env(), "unsuccessful, or one or more of the arguments are out of range").ThrowAsJavaScriptException();
        return info.Env().Undefined();
    }
    
    return Napi::Number::New(info.Env(), hresult);
}

Napi::Value PicoScope::setTrigger(const Napi::CallbackInfo &info){

    if(!connected()){
        Napi::Error().New(info.Env(), "Connect to device first").ThrowAsJavaScriptException();
        return info.Env().Undefined();
    }
    if(info.Length() < 5){
        Napi::Error().New(info.Env(), "at least 5 args needed").ThrowAsJavaScriptException();
        return info.Env().Undefined();
    }
    if(!info[0].IsNumber() || ((int32_t)info[0].As<Napi::Number>() != 0 && (int32_t)info[0].As<Napi::Number>() != 1 && (int32_t)info[0].As<Napi::Number>() != 5)){
        Napi::Error().New(info.Env(), "first arg has to be number -> channel A (0), B (1), NONE (5)").ThrowAsJavaScriptException();
        return info.Env().Undefined();
    }
    if(!info[1].IsNumber() || ((int32_t)info[1].As<Napi::Number>() < 0 && (int32_t)info[1].As<Napi::Number>() >= 16)){
        Napi::Error().New(info.Env(), "second arg has to be number -> 16 bit ADC - 0 index").ThrowAsJavaScriptException();
        return info.Env().Undefined();
    }
    if(!info[2].IsNumber() || ((int32_t)info[2].As<Napi::Number>() != 0 && (int32_t)info[2].As<Napi::Number>() != 1)){
        Napi::Error().New(info.Env(), "third arg has to be number -> RISING (0) - FALLING (1) ").ThrowAsJavaScriptException();
        return info.Env().Undefined();
    }
    if(!info[3].IsNumber() || ((int32_t)info[3].As<Napi::Number>() < -100 && (int32_t)info[3].As<Napi::Number>() > 100)){
        Napi::Error().New(info.Env(), "forth arg has to be number -> -100 <= x <= 100 ").ThrowAsJavaScriptException();
        return info.Env().Undefined();
    }
    if(!info[4].IsNumber() || (int32_t)info[4].As<Napi::Number>() < 0){

        Napi::Error().New(info.Env(), "fifth arg has to be number -> x >= 0 ms, x == 0 will wait 4eva");
        return info.Env().Undefined();
    }

    int16_t channel = (int32_t)info[0].As<Napi::Number>();
    int16_t threshold = (int32_t)info[1].As<Napi::Number>();
    int16_t direction = (int32_t)info[2].As<Napi::Number>();
    int16_t delay = (int32_t)info[3].As<Napi::Number>();
    int16_t trigger_ms = (int32_t)info[4].As<Napi::Number>();

    int16_t result = ps2000_set_trigger(deviceHandle, channel, threshold, direction, delay, trigger_ms);
    return Napi::Number::New(info.Env(), result);
}

Napi::Value PicoScope::stream(const Napi::CallbackInfo &info){

    if(!connected()){
        Napi::Error().New(info.Env(), "Connect to device first").ThrowAsJavaScriptException();
        return Napi::Boolean::New(info.Env(), false);
    }
    uint32_t sampleInterval = 1;
    PS2000_TIME_UNITS units = PS2000_TIME_UNITS::PS2000_US;
    uint32_t maxSamples = 1000; // 2 seconds

    int result = ps2000_run_streaming_ns(deviceHandle, sampleInterval, units, maxSamples, 0, 1, OVERVIEW_BUFFER_SIZE);

    if(TEST) printf("start streaming: %d\n\n", result);
    if(info.Length() < 1 || !info[0].IsFunction()){

        Napi::Error::New(info.Env(), "invalid argument 0").ThrowAsJavaScriptException();
        return info.Env().Undefined();
    }
    Napi::Function callback = info[0].As<Napi::Function>();
    DeviceState dState = {
        deviceHandle,
        &read,
        &A,
        &B,
    };
    StreamWorker *worker = new StreamWorker(callback, &dState);
    // yield
    if(TEST) printf("\n\n\nyield\n\n\n");
    read = 1;
    worker->Queue();
    return info.Env().Undefined();
}
Napi::Value PicoScope::stop(const Napi::CallbackInfo &info){
    read = false;
    return info.Env().Undefined();
}

Napi::Value PicoScope::disconnect(const Napi::CallbackInfo &info){

    this->stop(info);
    int16_t result = ps2000_close_unit(this->deviceHandle);
    if(TEST) printf("\n\n\n\n\n\n\n\n\nclose handle %d :: %d\n\n\n\n\n\n\n\n\n", deviceHandle, result);
    if(result == 0){

        return Napi::Boolean::New(info.Env(), false);
    } else {

        connectState &= ~1;
        return Napi::Boolean::New(info.Env(), true);
    }
}

PicoScope::PicoScope(const Napi::CallbackInfo &info)
: Napi::ObjectWrap<PicoScope>(info){

    deviceHandle = 0;
    A = {0, 0, 0, 0};
    B = {0, 0, 0, 0};    
}


