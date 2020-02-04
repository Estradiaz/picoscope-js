#include <napi.h>
// #include <ps2000.h>
using OverviewBuffer = void(
    int16_t **overviewBuffers, 
    int16_t overflow, 
    uint32_t triggeredAt, 
    int16_t triggered, 
    int16_t auto_stop, 
    uint32_t nValues
);

class PS : public Napi::ObjectWrap<PS> {

    public:
        static Napi::Object Init(Napi::Env env, Napi::Object exports);
        PS(const Napi::CallbackInfo& info);
        ~PS();
        void overviewBuffer(
            int16_t **overviewBuffers, 
            int16_t overflow, 
            uint32_t triggeredAt, 
            int16_t triggered, 
            int16_t auto_stop, 
            uint32_t nValues
        );
        int trampolineIndex;
        int asyncCalledCount;
        int estimatedCallCount;
        int agragatedValues;
        std::vector<int32_t> minA;
        std::vector<int32_t> maxA;
        std::vector<int32_t> minB;
        std::vector<int32_t> maxB;
        Napi::ThreadSafeFunction tsfn = NULL;

    private:
        static Napi::FunctionReference constructor;
        // static PS *trampoline[];
        
        Napi::Value connect(const Napi::CallbackInfo &info);
        Napi::Value disconnect(const Napi::CallbackInfo &info);
        Napi::Value setChannel(const Napi::CallbackInfo &info);
        Napi::Value setTrigger(const Napi::CallbackInfo &info);
        Napi::Value stream(const Napi::CallbackInfo &info);
        Napi::Value getTemplateIndex(const Napi::CallbackInfo &info);
        // Napi::Function setTrampolineCount(const Napi::CallbackInfo &info);
        int psHandle = 0;
        
        bool running = false;
        

        
};