declare interface PS {

    constructor()
    connect(): number
    setChannel(channel: 0 | 1, enabled: 0 | 1, coupling: 0 | 1, range: number) : number
    setTrigger(channel: 0| 1, adc: number, direction: 0 | 1, threshold: number, delay: number): number
    stream(cb: Function): boolean
    templateIndex(): number
}