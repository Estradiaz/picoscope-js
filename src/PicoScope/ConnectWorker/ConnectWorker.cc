#include "./ConnectWorker.h"
#include <ps2000.h>
#include "../../structs.h"

// ConnectWorker::ConnectWorker(Napi::Function callback, int16_t *handle)
//         : Napi::AsyncProgressWorker<ProgressEvent>(callback),
//         handle(handle){}

// ConnectWorker::~ConnectWorker(){}

void ConnectWorker::Execute(const ExecutionProgress &progress){

    if(!&handle){
        // Napi::Error::New(Env(), "no device selected - object invalid").ThrowAsJavaScriptException();
        errorFlag = true;
        error =  "no device selected - object invalid";
        if(TEST) printf("no device selected - object invalid\n\n");
        return;
    }
    int16_t result = ps2000_open_unit_async();
    if(TEST) printf("open unit async: %d\n\n", result != 0);
    if(result == 0){
        // Napi::Error::New(Env(), "no device selected - object invalid").ThrowAsJavaScriptException();
        errorFlag = true;
        error = "a previous open operation in progress";
        if(TEST) printf("a previous open operation in progress\n\n");
        return;
    }
    int16_t openProgress = 0;
    while(openProgress == 0){

        openProgress = ps2000_open_unit_progress(handle, &progressPercent);
        if(TEST) printf("process meta: %d, %d, %d\n\n\n\n\n\n", openProgress, *handle, progressPercent);
        ProgressEvent event = {*handle, progressPercent};
        if(event.handle != 0){
            *connectState |= 1;
        }
        progress.Send(&event, 1);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        // delete &event;
    }
    if(openProgress == -1){

        errorFlag = true;
        error = "the oscilloscope failed to open or was not found";
        return;
    } else {

        
    }
    // else {

    //     openProgress = ps2000_open_unit_progress(handle, &progressPercent);
    //     if(TEST) printf("process meta: %d, %d, %d\n\n\n\n\n\n", openProgress, *handle, progressPercent);
    //     ProgressEvent event = {*handle, progressPercent};
    //     progress.Send(&event, 1);
    // }
}

void ConnectWorker::OnProgress(const ProgressEvent *event, size_t size){

    Napi::HandleScope scope(Env());

    if(event->handle != 0){
        *connectState |= 1;
    }
    Napi::Object obj = Napi::Object::New(Env());
    obj.Set("deviceHandle", Napi::Number::New(Env(), event->handle));
    obj.Set("progressPercent", Napi::Number::New(Env(), event->progess));

    Callback().Call({
        obj,   
    });
}

void ConnectWorker::OnOk(){

    Napi::HandleScope scope(Env());

    if(errorFlag){
        Napi::Error::New(Env(), error).ThrowAsJavaScriptException();
        errorFlag = false;
        return;
    }

    // Napi::Object obj = Napi::Object::New(Env());
    // obj.Set("deviceHandle", Napi::Number::New(Env(), *handle));
    // obj.Set("progressPercent", Napi::Number::New(Env(), 100));
    // Callback().Call({
    //     obj
    // });
}