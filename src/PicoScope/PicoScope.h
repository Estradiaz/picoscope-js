#include "../structs.h"





class PicoScope: public Napi::ObjectWrap<PicoScope>{

    public:
        static Napi::Object Init(Napi::Env env, Napi::Object exports);
        PicoScope(const Napi::CallbackInfo &info);
        ~PicoScope(){};

        Napi::Value setChannel(const Napi::CallbackInfo &info);
        Napi::Value getChannel(const Napi::CallbackInfo &info);
        Napi::Value setTrigger(const Napi::CallbackInfo &info);
        Napi::Value stream(const Napi::CallbackInfo &info);
        Napi::Value stop(const Napi::CallbackInfo &info);
        Napi::Value disconnect(const Napi::CallbackInfo &info);

        bool connected();
        bool connecting();

    private:
        static Napi::FunctionReference constructor;
        Napi::Value connect(const Napi::CallbackInfo &info);
        // template<typename T>
        // std::vector<Napi::AsyncProgressWorker<T>> worker;
        int16_t deviceHandle = 0;
        int16_t connectState = 0;
        int16_t read = 0;
        Channel A = {0, 0, 0, 0};
        Channel B = {0, 0, 0, 0};
        // static const uint32_t OVERVIEW_BUFFER_SIZE;
};