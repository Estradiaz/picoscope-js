import { Channel, PicoScope } from "types";
import { EventEmitter } from "events";
const { PicoScope } = require ("bindings")("picoscope-js") as { "PicoScope": PicoScope }


export default class PicoScopeJS extends PicoScope {
    
    _emitter: EventEmitter = new EventEmitter();
    emit(event: string | symbol, ...args: any[]){

        return this._emitter.emit(event, ...args)
    }
    on(event: string | symbol, listener: (...args: any[]) => void){
        this._emitter.on(event, listener)
        return this
    }
    once(event: string | symbol, listener: (...args: any[]) => void){
        this._emitter.once(event, listener)
        return this
    }
    off(event: string | symbol, listener: (...args: any[]) => void){
        this._emitter.off(event, listener)
        return this
    }
    get eventNames(){
        return this._emitter.eventNames()
    }
    promise: {[key:string]: (...args: any[])=>Promise<any>} = {}
    constructor(){

        super();
        Object.defineProperty(this.promise, "connect", {
            "get": () => function(): Promise<number> {
                
                return new Promise((res, rej) => {
   
                    //@ts-ignore
                    this.connect((payload: {deviceHandle: number, progressPercent: number}) => {
    
                        if(payload.progressPercent === 100){
                            res(payload.deviceHandle)
                        }                        
                    })
                })
            }
        })
    }

    connect(callback?: Function): void{
        if(callback !== undefined){

            return super.connect((...args: any[]) => {
                if(args[0])
                callback(...args)
            }) as void
        } else {
            
            return super.connect((payload: {deviceHandle: number, progressPercent: number}) => {
                
                
                if(payload.progressPercent <= 100){

                    this.emit("connecting", payload.progressPercent)
                }
                if(payload.progressPercent === 100){
                    this.emit("connected", payload.deviceHandle)
                }
                // this.emit("connected", args)
            }) as void
        }
        
    }
    connectSync(): number{
        return super.connect() as number
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
    stream(callback?: Function){
        if(callback){

            return super.stream(callback);
        } else {

            return super.stream((...args: any[]) => {
                this.emit("data", args)
            })
        }
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
