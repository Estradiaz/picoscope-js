#include "./StreamWorker.h"
#include <ps2000.h>
#include <conio.h>




template<const unsigned int I>void f(
    short **overviewBuffers, 
    int16_t overflow, 
    uint32_t triggeredAt, 
    int16_t triggered, 
    int16_t auto_stop, 
    uint32_t nValues
) {
    
    if(TEST) printf("f<%d>: nValues :: %d\n\n", I, nValues);
    if(nValues < 2){
        return;
    }
    if(nValues > OVERVIEW_BUFFER_SIZE){
        if(TEST) printf("overview buffer size exeeds: %d with %d", OVERVIEW_BUFFER_SIZE, nValues);
        return;
    }
    // can lead to access violation due to inactive channel
    std::vector<short> maxA({});
    std::vector<short> minA({});
    std::vector<short> maxB({});
    std::vector<short> minB({});
    if(StreamWorker::channels[I]->A->enabled){

        // maxA.insert(maxA.end(), &overviewBuffers[0][0], &overviewBuffers[0][nValues - 1]);
        maxA = std::vector<short>(&overviewBuffers[0][0], &overviewBuffers[0][nValues - 1]);
        minA = std::vector<short>(&overviewBuffers[1][0], &overviewBuffers[1][nValues - 1]);
    }
    if(StreamWorker::channels[I]->B->enabled){

        maxB = std::vector<short> (&overviewBuffers[2][0], &overviewBuffers[2][nValues - 1]);
        minB = std::vector<short> (&overviewBuffers[3][0], &overviewBuffers[3][nValues - 1]);
    }

    const OverviewBufferLists args = {
        nValues,
        maxA,
        minA,
        maxB,
        minB
    };
        
    
    if(TEST) printf("\n\ntrampoline: %d\n\n", I);
    StreamWorker::progesses[I]->Send(&args, 1 );
    if(TEST) printf("\n\npost trampoline: %d\n\n", I);
}

const GetOverviewBuffersMaxMin StreamWorker::fptr[N] = {
    &f<0>, &f<1>, &f<2>, &f<3>, &f<4>, &f<5>, &f<6>, &f<7>, &f<8>, &f<9>,
    &f<10>, &f<11>, &f<12>, &f<13>, &f<14>, &f<15>, &f<16>, &f<17>, &f<18>, &f<19>
    // &f<20>, &f<21>, &f<22>, &f<23>, &f<24>, &f<25>, &f<26>, &f<27>, &f<28>, &f<29>
};

void StreamWorker::Execute(const ExecutionProgress& progess){

    StreamWorker::progesses[instanceId] = &progess;
    const Channels c = {
        channelA,
        channelB
    };
    StreamWorker::channels[instanceId] = &c;
    if(!*read){
        Napi::Error::New(Env(), "no device selected - object invalid").ThrowAsJavaScriptException();
        return;
    }
    // // charge 
    while (*read && !_kbhit())
    {
        if(TEST) printf("\n\n\nwhile read: %d\n\n\n", (int)*read);
        // std::this_thread::sleep_for(std::chrono::seconds(1));
        int result = ps2000_get_streaming_last_values(deviceHandle, fptr[this->instanceId]);
        if(TEST) printf("\n\nget last values: %d\n\n\n", result);
        if(result == 0){
            // *read = false;
            if(TEST) printf("\n\n\nwhile read: %d :: %d\n\n\n", (int)*read, result);
            // return;
        } else {
            // if(TEST) printf("\n\nget last values: %d\n\n\n", result);
        }
        // *read = false;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        if(TEST) printf("sleep end\n\n");
    }
    if(TEST) printf("execute ends");
    // if(TEST) printf("\n\n\nwhile read: %d\n\n\n", (int)*read);
    // ps2000_get_streaming_last_values(this->deviceHandle, fptr[this->instanceId]);
    return;
}

void StreamWorker::OnOk(){

    Napi::HandleScope scope(Env());
    
    Callback().Call({
        Napi::Boolean::New(Env(), true)
    });
}

uint64_t totalDataCount = 0;
void StreamWorker::OnProgress(const OverviewBufferLists *args, size_t size){

    Napi::HandleScope scope(Env());

    
    if(args){

        if(TEST) printf("\n\n\n\n\n\n\non progress: %d :: %d\n\n", args->nValues, totalDataCount);
    } else {
        if(TEST) printf("\n\n\n\n\n\n\nargs invalid");
        return;
    }
    totalDataCount += args->nValues;

    std::vector<int16_t> minA(args->minA.begin(), args->minA.end());
    std::vector<int16_t> maxA(args->maxA.begin(), args->maxA.end());
    std::vector<int16_t> minB(args->minB.begin(), args->minB.end());
    std::vector<int16_t> maxB(args->maxB.begin(), args->maxB.end());

    Napi::Object obj = Napi::Object::New(Env());
    obj.Set("nValues", Napi::Number::New(Env(), args->nValues));
    // obj.Set("total", Napi::Number::New(Env(), args->nValues));

    obj.Set("minA", Napi::ArrayBuffer::New(Env(), (void *)minA.data(), sizeof(int16_t) * minA.size()));
    obj.Set("maxA", Napi::ArrayBuffer::New(Env(), (void *)maxA.data(), sizeof(int16_t) * maxA.size()));
    obj.Set("minB", Napi::ArrayBuffer::New(Env(), (void *)minB.data(), sizeof(int16_t) * minB.size()));
    obj.Set("maxB", Napi::ArrayBuffer::New(Env(), (void *)maxB.data(), sizeof(int16_t) * maxB.size()));

    if(TEST) printf("prepared transfer obj\n\n");

    Callback().Call({
        obj,
        // Napi::Number::New(Env(), args->nValues),
        // Napi::Number::New(Env(), 1)
    });
    if(TEST) printf("progress callback called\n\n");
    return;
}


const Napi::AsyncProgressWorker<OverviewBufferLists>::ExecutionProgress* StreamWorker::progesses[N] = {};
const Channels* StreamWorker::channels[N] = {};
bool StreamWorker::claimedIndex[N] = {};
unsigned int StreamWorker::getNextEmptyIndex(){
    for (unsigned i = 0; i < N; i++){

        if(!claimedIndex[i]){
            claimedIndex[i] = true;
            return i;
        }
    }
    return -1;
}
void StreamWorker::setIndexEmpty(unsigned int index){

    claimedIndex[index] = false;
}

