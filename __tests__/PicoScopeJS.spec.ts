import PicoScopeJS from "../index";

describe("Picoscope napi implementation", function(){
    const ps = new PicoScopeJS()
    it("should be a function ", function(){

        expect(typeof PicoScopeJS).toBe("function")
    })
    it("should be a ctor function", function(){

        expect(typeof (ps)).toBe("object")
    })

    describe("PS2000er connected", function(){


        afterAll(function(){
            // ps.disconnect()
        })
        describe("connection sets", function(){

            it("should connect blocking", function(){

                let deviceHandle = ps.connectSync() as number
                expect(deviceHandle).toBeGreaterThan(0);
                ps.disconnect()
            })
            it("should connect async passing a callback", function(done){

                let deviceHandle = ps.connect((...args: any[]) => {

                    console.log(args[0])
                    if(args[0].progressPercent === 100){
                        expect(args[0].deviceHandle).toBeGreaterThan(0)
                        ps.disconnect()
                        done()
                    }
                })
            })
            it("should connect emitting connected event", function(done){

                ps.once("connected", ()=> {
                    ps.disconnect()
                    done()
                })
                ps.connect()

            })
            // it("should connect returning an promise<number>", async function(){

            //     let deviceHandle = await ps.promise.connect()
            //     expect(deviceHandle).toBeGreaterThan(0)
            //     ps.disconnect()
            // })
        })

        it("should connect to device", function(done){

            expect((ps.connectSync())).toBeGreaterThan(0);
        })
        
        
        it("should setChannelA", function(){
            
            expect(ps.setChannelA(true, true, 4)).toBeGreaterThan(0)
            expect(ps.channelA).toEqual({
                name: "A",
                enabled: 1,
                dc: 1,
                range: 4,
                direction: 0,
            })
        })
        it("should setChannelB", function(){
            
            expect(ps.setChannelB(false, true, 4)).toBeGreaterThan(0)
            expect(ps.channelB).toEqual({
                name: "B",
                enabled: 0,
                dc: 1,
                range: 4,
                direction: 0,
            })
        })

        it("should setTrigger", function(){

            expect(ps.setTrigger(0, 1, 1, 1, 1)).toBeGreaterThan(0)
        })
        for(let repeat = 0; repeat < 3; repeat++)
        it("should start reading and stop", function(done){

            let counter = 0;
            expect(ps.stream(function(){
                if(counter++ == 10){
                    ps.stop();
                    done()
                }
            })).not.toBe(0)
        }, 20000 )



        it("should disconnect", function(){

            expect(ps.disconnect()).toBe(true);
        })
    })
})