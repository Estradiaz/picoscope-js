#include "./ps_wrap.h"
#include <ps2000.h>

const int OVERVIEW_BUFFER_SIZE = 20000;

struct OverviewBufferArgument {
    short **overviewBuffers;
    short *overviewBuffersAMax;
    short *overviewBuffersAMin;
    short *overviewBuffersBMax;
    short *overviewBuffersBMin;
    int16_t overflow;
    uint32_t triggeredAt; 
    int16_t triggered;
    int16_t auto_stop; 
    uint32_t nValues;
};

const int N = 20;
PS *trampoline[N] = {};
template<int I>void f(
    short **overviewBuffers, 
    int16_t overflow, 
    uint32_t triggeredAt, 
    int16_t triggered, 
    int16_t auto_stop, 
    uint32_t nValues
) {

    printf("\nentry f<%d>: %d\n\n", I, overviewBuffers[0][1]);
    OverviewBufferArgument args = {};
    args.overviewBuffers = overviewBuffers;
    args.overviewBuffersAMax = overviewBuffers[0];
    args.overviewBuffersAMin = overviewBuffers[1];
    args.overviewBuffersBMax = overviewBuffers[0];
    args.overviewBuffersBMin = overviewBuffers[1];
    args.overflow = overflow;
    args.triggeredAt = triggeredAt;
    args.triggered = triggered;
    args.auto_stop = auto_stop;
    args.nValues = nValues;

    printf("\nnValues: %d\n\n", args.nValues);

    PS* _trampoline = trampoline[I];

    int16_t *rtnArr1 = new int16_t[args.nValues];
    int16_t *rtnArr2 = new int16_t[args.nValues];

    if(args.overviewBuffersAMax){

        printf("deref overviewBuffersAMax works\n %d\n", _trampoline->maxA.size(), sizeof(int16_t) * args.nValues);
        // memcpy_s((void *)rtnArr1, sizeof(int16_t) , args.overviewBuffersAMax, sizeof(int16_t));
        _trampoline->maxA.insert(_trampoline->maxA.end(), args.overviewBuffersAMax, &args.overviewBuffersAMax[args.nValues - 1]);
        printf("memcpy works\n");
    }
    if(args.overviewBuffersAMin){

        printf("deref overviewBuffersAMin works\n");
        // memcpy_s((void *)rtnArr2, sizeof(int16_t), args.overviewBuffersAMin, sizeof(int16_t));
        _trampoline->minA.insert(_trampoline->minA.end(), args.overviewBuffersAMin, &args.overviewBuffersAMin[args.nValues - 1]);
        printf("memcpy works\n");
    }
    if(args.overviewBuffersBMax){

        printf("deref overviewBuffersAMax works\n %d\n", _trampoline->maxA.size(), sizeof(int16_t) * args.nValues);
        // memcpy_s((void *)rtnArr1, sizeof(int16_t) , args.overviewBuffersAMax, sizeof(int16_t));
        _trampoline->maxB.insert(_trampoline->maxB.end(), args.overviewBuffersBMax, &args.overviewBuffersBMax[args.nValues - 1]);
        printf("memcpy works\n");
    }
    if(args.overviewBuffersBMin){

        printf("deref overviewBuffersAMin works\n");
        // memcpy_s((void *)rtnArr2, sizeof(int16_t), args.overviewBuffersAMin, sizeof(int16_t));
        _trampoline->minB.insert(_trampoline->minB.end(), args.overviewBuffersBMin, &args.overviewBuffersBMin[args.nValues - 1]);
        printf("memcpy works\n");
    }

    args.overviewBuffersAMax = rtnArr1;
    args.overviewBuffersAMin = rtnArr2;

    napi_status result = (napi_status)-999;
    
    if(_trampoline!=NULL)
    result = _trampoline->tsfn.BlockingCall(&args, [_trampoline](Napi::Env env, Napi::Function func, OverviewBufferArgument* val){

        OverviewBufferArgument args = *val;
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("maxA", Napi::ArrayBuffer::New(env, &_trampoline->maxA[0], sizeof(int16_t) * _trampoline->maxA.size()));
        obj.Set("minA", Napi::ArrayBuffer::New(env, &_trampoline->minA[0], sizeof(int16_t) * _trampoline->minA.size()));
        obj.Set("maxB", Napi::ArrayBuffer::New(env, &_trampoline->maxB[0], sizeof(int16_t) * _trampoline->maxB.size()));
        obj.Set("minB", Napi::ArrayBuffer::New(env, &_trampoline->minB[0], sizeof(int16_t) * _trampoline->minB.size()));
        obj.Set("type", Napi::String::New(env, "int16_t"));
        obj.Set("overflowA", Napi::Boolean::New(env, 1 & args.overflow));
        obj.Set("overflowB", Napi::Boolean::New(env, 2 & args.overflow));
        printf("callback calling\n");
        func.Call({
            obj
        });
        _trampoline->agragatedValues += args.nValues;
        while(_trampoline->agragatedValues >= OVERVIEW_BUFFER_SIZE){

            _trampoline->agragatedValues -= OVERVIEW_BUFFER_SIZE;
            _trampoline->asyncCalledCount += 1;
            printf("\n\ncallback done: %d\n\n\n", _trampoline->asyncCalledCount);
        }
        if(_trampoline && (_trampoline->asyncCalledCount == _trampoline->estimatedCallCount)){

            // printf("start tsfn release\n");
            // napi_status released = _trampoline->tsfn.Release();
            // if(released == napi_invalid_arg){

            //     printf("tsfn released error: %d\ninvalid arg\n", released);
            // } else {

            //     printf("tsfn released: %d\n", released);
            // }
        }
    });
    printf("napi_status: %d\n", result);
}
void testCall(
    int16_t **overviewBuffers, 
    int16_t overflow, 
    uint32_t triggeredAt, 
    int16_t triggered, 
    int16_t auto_stop, 
    uint32_t nValues
){
    printf("testcall:%d", nValues);
}

const OverviewBuffer * fptr_array[N] = {
    &f<0>,
    &f<1>,
    &f<2>,
    &f<3>,
    &f<4>,
    &f<5>,
    &f<6>,
    &f<7>,
    &f<8>,
    &f<9>,
    &f<10>,
    &f<11>,
    &f<12>,
    &f<13>,
    &f<14>,
    &f<15>,
    &f<16>,
    &f<17>,
    &f<18>,
    &f<19>,
};
// template<int I> struct fptr_init{
//     const OverviewBuffer *fptr = fptr_array[I] = &f<I>;
//     const fptr_init<I - 1> recurse;
// };
// template<> struct fptr_init<-1> {};


int lastUsedFPtr = 0;


OverviewBuffer * testCallPtr = &testCall;

void callback(Napi::Env env, Napi::Function jsCallback, OverviewBufferArgument *value){
    // jsCallback.Call({Napi::Number::New(env, *value)});
};
void getStreamingLastValuesFailed(Napi::Env env, Napi::Function jsCallback, int *value){
    // Napi::Error().New(env, "get_streaming_last_values failed -> problem or value out of range").ThrowAsJavaScriptException();
    jsCallback.Call({Napi::String::New(env, "error")});
    
};

Napi::FunctionReference PS::constructor;

Napi::Object PS::Init(Napi::Env env, Napi::Object exports){
    Napi::HandleScope scope(env);

    Napi::Function func = DefineClass(
        env, 
        "PS", 
        {
            InstanceMethod("connect", &PS::connect),
            InstanceMethod("disconnect", &PS::disconnect),
            InstanceMethod("setChannel", &PS::setChannel),
            InstanceMethod("setTrigger", &PS::setTrigger),
            InstanceMethod("stream", &PS::stream),
            InstanceMethod("templateIndex", &PS::getTemplateIndex)
        }
    );
    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();

    exports.Set("PS", func);
    return exports;
}
Napi::Value PS::disconnect(const Napi::CallbackInfo &info){
    if(this->psHandle == 0){
        return Napi::Boolean::New(info.Env(), true);
    }
    if(this->tsfn)
        this->tsfn.Release();
    ps2000_close_unit(this->psHandle);
    this->psHandle = 0;
    return Napi::Boolean::New(info.Env(), true);
}
Napi::Value PS::getTemplateIndex(const Napi::CallbackInfo &info){

    return Napi::Number::New(info.Env(), this->trampolineIndex);
}

Napi::Value PS::connect(const Napi::CallbackInfo &info){
    
    int unit = ps2000_open_unit();
    if(unit < 0){

        Napi::Error().New(info.Env(), "the oscilloscope fails to open").ThrowAsJavaScriptException();
        return Napi::Number::New(info.Env(), unit);
    }
    if(unit == 0){

        Napi::Error().New(info.Env(), "no oscilloscope is found").ThrowAsJavaScriptException();
        return Napi::Number::New(info.Env(), unit);
    }

    this->psHandle = unit;
    return Napi::Number::New(info.Env(), unit);
}

Napi::Value PS::setChannel(const Napi::CallbackInfo &info){

    int unit = this->psHandle;
    if(unit <= 0){
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

    uint16_t channel = (uint32_t)info[0].As<Napi::Number>();
    uint16_t enabled = (uint32_t)info[1].As<Napi::Number>();
    uint16_t coupling = (uint32_t)info[2].As<Napi::Number>();
    uint16_t range = (uint32_t)info[3].As<Napi::Number>();

    int hresult = ps2000_set_channel(unit, channel, enabled, coupling, range);
    // while(hresult == 0){
    //     hresult = ps2000_set_channel(unit, channel, enabled, coupling, range);
    // }
    return Napi::Number::New(info.Env(), hresult);
}

Napi::Value PS::setTrigger(const Napi::CallbackInfo &info){

    int unit = this->psHandle;
    if(unit <= 0){
        Napi::Error().New(info.Env(), "Connect to device first").ThrowAsJavaScriptException();
        return Napi::Number::New(info.Env(), -1);
    }
    if(info.Length() < 5){
        Napi::Error().New(info.Env(), "at least 5 args needed").ThrowAsJavaScriptException();
        return Napi::Number::New(info.Env(), -1);
    }
    if(!info[0].IsNumber() || ((int32_t)info[0].As<Napi::Number>() != 0 && (int32_t)info[0].As<Napi::Number>() != 1 && (int32_t)info[0].As<Napi::Number>() != 5)){
        Napi::Error().New(info.Env(), "first arg has to be number -> channel A (0), B (1), NONE (5)").ThrowAsJavaScriptException();
        return Napi::Number::New(info.Env(), -1);
    }
    if(!info[1].IsNumber() || ((int32_t)info[1].As<Napi::Number>() < 0 && (int32_t)info[1].As<Napi::Number>() >= 16)){
        Napi::Error().New(info.Env(), "second arg has to be number -> 16 bit ADC - 0 index").ThrowAsJavaScriptException();
        return Napi::Number::New(info.Env(), -1);
    }
    if(!info[2].IsNumber() || ((int32_t)info[2].As<Napi::Number>() != 0 && (int32_t)info[2].As<Napi::Number>() != 1)){
        Napi::Error().New(info.Env(), "third arg has to be number -> RISING (0) - FALLING (1) ").ThrowAsJavaScriptException();
        return Napi::Number::New(info.Env(), -1);
    }
    if(!info[3].IsNumber() || ((int32_t)info[3].As<Napi::Number>() < -100 && (int32_t)info[3].As<Napi::Number>() > 100)){
        Napi::Error().New(info.Env(), "forth arg has to be number -> -100 <= x <= 100 ").ThrowAsJavaScriptException();
        return Napi::Number::New(info.Env(), -1);
    }
    if(!info[4].IsNumber() || (int32_t)info[4].As<Napi::Number>() < 0){

        Napi::Error().New(info.Env(), "fifth arg has to be number -> x >= 0 ms, x == 0 will wait 4eva");
        return Napi::Number::New(info.Env(), -1);
    }

    int16_t channel = (int32_t)info[0].As<Napi::Number>();
    int16_t threshold = (int32_t)info[1].As<Napi::Number>();
    int16_t direction = (int32_t)info[2].As<Napi::Number>();
    int16_t delay = (int32_t)info[3].As<Napi::Number>();
    int16_t trigger_ms = (int32_t)info[4].As<Napi::Number>();

    int16_t result = ps2000_set_trigger(unit, channel, threshold, direction, delay, trigger_ms);
    return Napi::Number::New(info.Env(), result);
}

std::thread nativeThread;

Napi::Value PS::stream(const Napi::CallbackInfo &info){

    int unit = this->psHandle;
    if(unit <= 0){
        Napi::Error().New(info.Env(), "Connect to device first").ThrowAsJavaScriptException();
        return Napi::Boolean::New(info.Env(), false);
    }
    uint32_t sampleInterval = 1;
    PS2000_TIME_UNITS units = PS2000_TIME_UNITS::PS2000_US;
    uint32_t maxSamples = 1000; // 2 seconds

    int result = ps2000_run_streaming_ns(unit, sampleInterval, units, maxSamples, 0, 1, OVERVIEW_BUFFER_SIZE);
    // return Napi::Number::New(info.Env(), result);
    if(result == 0){
        Napi::Error().New(info.Env(), "run_streamin_ns failed -> problem or value out of range").ThrowAsJavaScriptException();
        return Napi::Boolean::New(info.Env(), false);
    }

    this->tsfn = Napi::ThreadSafeFunction::New(
        info.Env(),
        info[0].As<Napi::Function>(),
        "resName",
        0,
        1,
        [](Napi::Env){
            nativeThread.join();
        }
    );
    Napi::Env env = info.Env();
    nativeThread = std::thread([env, unit, this]{

        int loopcount = 12;
        this->asyncCalledCount = 0;
        this->agragatedValues = 0; // just a little above due to offset
        this->estimatedCallCount = 0;
        for(int i = 0; i < loopcount; i++){

            
            std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
            this->estimatedCallCount += 1;
            printf("read stream: %d added -> %d\n", 1, estimatedCallCount);
            int16_t result = ps2000_get_streaming_last_values(unit, fptr_array[this->trampolineIndex]);
            // int* val = new int(result);
            if(result == 0){

                this->estimatedCallCount -= 1;
                printf("read stream: %d removed -> %d\n", 1, this->estimatedCallCount);
                // this->tsfn.NonBlockingCall(val, getStreamingLastValuesFailed);
            }
            
        }
        // this->tsfn.NonBlockingCall(&(this->trampolineIndex), [](Napi::Env env, Napi::Function func, int * val){
        //     func.Call({Napi::Number::New(env, *val)});
        // });
        if(this && (this->asyncCalledCount == this->estimatedCallCount)){

            printf("start tsfn release\n");
            // napi_status released = this->tsfn.Release();
            // if(released == napi_invalid_arg){

            //     printf("tsfn released error: %d\ninvalid arg\n", released);
            // } else {

            //     printf("tsfn released: %d\n", released);
            // }
        };
        
    });
    return Napi::Boolean::New(info.Env(), true);
}
PS::PS(const Napi::CallbackInfo &info) : Napi::ObjectWrap<PS>(info){

    for(int i = 0; i < N; i++){
        if(trampoline[i] == NULL){

            this->trampolineIndex = i;
            trampoline[i] = this;
            return;
        }
    }
    Napi::Error::New(info.Env(), "max trampoline callbacks assigned - wait for gc or disconnect").ThrowAsJavaScriptException();
}
PS::~PS(){

    printf("destructor called\n");
    this->tsfn.Release();
    ps2000_close_unit(this->psHandle);
    this->psHandle = 0;
    trampoline[this->trampolineIndex] = NULL;
}

void PS::overviewBuffer(
    int16_t **overviewBuffers, 
    int16_t overflow, 
    uint32_t triggeredAt, 
    int16_t triggered, 
    int16_t auto_stop, 
    uint32_t nValues
){

    OverviewBufferArgument args = {};
    args.overviewBuffers = overviewBuffers;
    args.overflow = overflow;
    args.triggeredAt = triggeredAt;
    args.triggered = triggered;
    args.auto_stop = auto_stop;
    args.nValues = nValues;

    this->tsfn.BlockingCall(&args, callback);
    this->running = false;
}