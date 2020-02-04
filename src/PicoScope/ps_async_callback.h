#include <napi.h>
#include <ps2000.h>

const unsigned int N = 20;

struct OverviewBufferLists {
    uint32_t nValues;
    std::vector<int16_t> minA;
    std::vector<int16_t> minB;
    std::vector<int16_t> maxA;
    std::vector<int16_t> maxB;
};

class PSAsyncCallback : public Napi::AsyncProgressWorker<OverviewBufferLists> {

    public:
        PSAsyncCallback(Napi::Function& callback, int16_t deviceHandle)
        : Napi::AsyncProgressWorker<OverviewBufferLists> (callback), 
            deviceHandle(deviceHandle), 
            delay(delay),
            instanceId(getNextEmptyIndex()) {}

        ~PSAsyncCallback(){
            setIndexEmpty(instanceId);
        }

        void Execute(const ExecutionProgress& progress);
        void OnProgress(OverviewBufferLists* args, size_t size);
        void OnOk();
        static const ExecutionProgress* progesses[N];
        static const GetOverviewBuffersMaxMin fptr[N];
        static bool claimedIndex[N];
        static unsigned int getNextEmptyIndex(){
            for (unsigned i = 0; i < N; i++){

                if(!claimedIndex[i]){
                    claimedIndex[i] = true;
                    return i;
                }
            }
            return -1;
        }
        static void setIndexEmpty(unsigned int index){

            claimedIndex[index] = false;
        }

    private:
        unsigned int instanceId;
        unsigned int delay;
        int deviceHandle; 
};