import { EventEmitter } from "events";
import { Stream } from "stream";

export type Channel = {
    name: string;
    dc: number;
    direction: number;
    range: number;
    enabled: number;
}
export interface PicoScope {
    
    new (): PicoScope;
    // connect(): number
    // connect(callback: Function): void
    connect(callback?: Function): void | number;
    setChannel(channel: 0 | 1, enabled: 0 | 1, coupling: 0 | 1, range: number): number;
    setTrigger(channel: 0 | 1 | 5, adc: number, direction: 0 | 1, threshold: number, delay: number): number;
    stream(cb: Function): boolean;
    stop(): void;
    disconnect(): boolean;
    getChannel(channel: number): Channel;
}
