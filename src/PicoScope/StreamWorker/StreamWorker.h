#include "../../structs.h"
#include <ps2000.h>


const int N = 20;




// volatile OverviewBufferLists oBL[N] = {};


class StreamWorker : public Napi::AsyncProgressWorker<OverviewBufferLists> {

    public:
        StreamWorker(Napi::Function& callback, DeviceState *deviceState)
        : Napi::AsyncProgressWorker<OverviewBufferLists> (callback), 
            deviceHandle(deviceState->deviceHandle), 
            delay(delay),
            read(deviceState->read),
            channelA(deviceState->A),
            channelB(deviceState->B),
            instanceId(getNextEmptyIndex()) {
                if(TEST) printf("\n\nstreamworker constructor\n\n");
            }

        ~StreamWorker(){
            setIndexEmpty(instanceId);
        }

        void Execute(const ExecutionProgress& progress);
        void OnProgress(const OverviewBufferLists* args, size_t size);
        void OnOk();
        static const ExecutionProgress* progesses[N];
        static const GetOverviewBuffersMaxMin fptr[N];
        
        static const Channels* channels[N];
        static bool claimedIndex[N];
        static unsigned int getNextEmptyIndex();
        static void setIndexEmpty(unsigned int index);

    private:
        unsigned int instanceId;
        unsigned int delay;
        int deviceHandle; 
        int16_t *read;
        Channel *channelA;
        Channel *channelB;
};