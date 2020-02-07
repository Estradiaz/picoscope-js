#include <napi.h>

using GetProgressEvent = void (int16_t* handle, int16_t* progressPercent);

struct ProgressEvent {
    int16_t handle;
    int16_t progess;
};

class ConnectWorker: public Napi::AsyncProgressWorker<ProgressEvent> {
    public:
        ConnectWorker(Napi::Function callback, int16_t *handle, int16_t *connectState)
        : Napi::AsyncProgressWorker<ProgressEvent>(callback),
        handle(handle),
        connectState(connectState),
        progressPercent(0),
        errorFlag(false),
        error("something unexpected")
        {}
        ~ConnectWorker(){
        }
        void Execute(const ExecutionProgress& progress);
        void OnProgress(const ProgressEvent* event, size_t size);
        void OnOk();

    private:
        int16_t *handle;
        int16_t *connectState;
        int16_t progressPercent;
        bool errorFlag;
        std::string error;

};