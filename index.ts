const {PicoScope} = require ("bindings")("picoscope-js")

const PS = PicoScope
export default class PicoScopeJS extends PS{
    connect(callback?: Function){
        if(callback){

            return super.connect(callback)
        }
        return super.connect()
    }
    setChannel(channel: 0 | 1, enabled: 0 | 1, coupling: 0 | 1, range: number): number {

        return super.setChannel(channel, enabled, coupling, range)
    }
    setChannelA(enabled: boolean, coupling: boolean, range: number){
        return super.setChannel(0, +enabled, +coupling, range)
    }
    setChannelB(enabled: boolean, coupling: boolean, range: number){
        return super.setChannel(1, +enabled, +coupling, range)
    }
    setTrigger(channel: 0| 1 | 5, adc: number, direction: 0 | 1, threshold: number, delay: number): number {
        return super.setTrigger(channel, adc, direction, threshold, delay)
    }
    setTriggerOff(){
        return super.setTrigger(5, 0,0,0,1);
    }
    stream(cb: Function){

        return super.stream(function(){
            cb(arguments[0])
        })
    }
    stop(){
        return super.stop();
    }
    disconnect(){
        return super.disconnect();
    }
    get templateIndex(){
        return super.templateIndex()
    }
    get channelA(){

        return super.getChannel(0)
    }
    get channelB(){

        return super.getChannel(1)
    }
}

// const ps = new PicoScopeJS()
// ps.connect()
// ps.setChannelA(true, true, 1)
// ps.setTrigger(0,1,0,0,1)
// ps.stream(function(){
//     console.log("1",arguments)
// })
// ps.stream(function(){
//     console.log("2", arguments)
// })
