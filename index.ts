import { Channel, PicoScope } from "types";
const { PicoScope } = require ("bindings")("picoscope-js") as { "PicoScope": PicoScope }


export default class PicoScopeJS extends PicoScope{
    
    promise = {
        connect(): Promise<number> {
            
            return new Promise((res, rej) => {

                super.connect((payload: {deviceHandle: number, progressPercent: number}) => {

                    if(payload.progressPercent === 100){
                        res(payload.deviceHandle)
                    }                        
                })
            })
        }
    }

    connect(callback?: Function){
        if(callback){

            return super.connect((payload: {deviceHandle: number, progressPercent: number}) => {
                
                if(payload.progressPercent <= 100){

                    this.emit("connecting", payload.progressPercent)
                }
                if(payload.progressPercent === 100){
                    this.emit("connected", payload.deviceHandle)
                }
                // this.emit("connected", args)
            }) 
        } else {

            return super.connect()
        }
        
    }
    connectSync(){
        
    }
    setChannel(channel: 0 | 1, enabled: 0 | 1, coupling: 0 | 1, range: number): number {

        return super.setChannel(channel, enabled, coupling, range)
    }
    setChannelA(enabled: boolean, coupling: boolean, range: number){
        return super.setChannel(0, +enabled as 0 | 1, +coupling as 0 | 1, range)
    }
    setChannelB(enabled: boolean, coupling: boolean, range: number){
        return super.setChannel(1, +enabled as 0 | 1, +coupling as 0 | 1, range)
    }
    setTrigger(channel: 0| 1 | 5, adc: number, direction: 0 | 1, threshold: number, delay: number): number {
        return super.setTrigger(channel, adc, direction, threshold, delay)
    }
    setTriggerOff(){
        return super.setTrigger(5, 0,0,0,1);
    }
    stream(){

        return super.stream((...args: any[]) => {
            this.emit("data", args)
        })
    }
    stop(){
        return super.stop();
    }
    disconnect(){
        return super.disconnect();
    }
    get channelA(){

        return super.getChannel(0)
    }
    get channelB(){

        return super.getChannel(1)
    }
}
