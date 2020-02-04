#include "./ps_async_callback.h"
#include <ps2000.h>


template<const unsigned int I>void f(
    short **overviewBuffers, 
    int16_t overflow, 
    uint32_t triggeredAt, 
    int16_t triggered, 
    int16_t auto_stop, 
    uint32_t nValues
) {
    const OverviewBufferLists args = {};
        // args.auto_stop = auto_stop;
        // args.overflow = overflow;
        // args.triggerAt = triggeredAt;
        // args.trigger = triggered;
        args.nValues = nValues;
        args.maxA.insert(args.maxA.end(), overviewBuffers[0], &overviewBuffers[0][nValues - 1]);
        args.maxB.insert(args.maxB.end(), overviewBuffers[1], &overviewBuffers[1][nValues - 1]);
        args.minA.insert(args.minA.end(), overviewBuffers[2], &overviewBuffers[2][nValues - 1]);
        args.minB.insert(args.minB.end(), overviewBuffers[3], &overviewBuffers[3][nValues - 1]);
    
    PSAsyncCallback::progesses[I]->Send(args, sizeof(args));
}

const GetOverviewBuffersMaxMin PSAsyncCallback::fptr[N] = {
    &f<0>, &f<1>, &f<2>, &f<3>, &f<4>, &f<5>, &f<6>, &f<7>, &f<8>, &f<9>,
    &f<10>, &f<11>, &f<12>, &f<13>, &f<14>, &f<15>, &f<16>, &f<17>, &f<18>, &f<19>
    // &f<20>, &f<21>, &f<22>, &f<23>, &f<24>, &f<25>, &f<26>, &f<27>, &f<28>, &f<29>
};

void PSAsyncCallback::Execute(const ExecutionProgress& progess){

    PSAsyncCallback::progesses[0] = &progess;
    if(!deviceHandle){
        Napi::Error::New(Env(), "no device selected - object invalid").ThrowAsJavaScriptException();
        return;
    }
    // charge 
    std::this_thread::sleep_for(std::chrono::seconds(this->delay));
    if(ps2000_get_streaming_last_values(this->deviceHandle, fptr[this->instanceId]) == 0){

        return;
    } else {
        
        return;
    }
}

void PSAsyncCallback::OnOk(){

    Napi::HandleScope scope(Env());
    Callback().Call({
        Env().Null(),
        Napi::Boolean::New(Env(), true)
    });
}

void PSAsyncCallback::OnProgress(OverviewBufferLists *args, size_t size){

    Napi::HandleScope scope(Env());

    Napi::Object obj = Napi::Object::New(Env());
    obj.Set("minA", Napi::ArrayBuffer::New(Env(), &args->minA[0], sizeof(int16_t) * args->minA.size()));
    obj.Set("minB", Napi::ArrayBuffer::New(Env(), &args->minB[0], sizeof(int16_t) * args->minB.size()));
    obj.Set("maxA", Napi::ArrayBuffer::New(Env(), &args->maxA[0], sizeof(int16_t) * args->maxA.size()));
    obj.Set("maxB", Napi::ArrayBuffer::New(Env(), &args->maxB[0], sizeof(int16_t) * args->maxB.size()));

    Callback().Call({
        Env().Null(),
        Env().Null(),
        obj
    });
}

