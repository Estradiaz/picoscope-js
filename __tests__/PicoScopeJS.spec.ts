import PicoScopeJS from "../index";

describe("Picoscope napi implementation", function(){
    const ps = new PicoScopeJS()
    it("should be a function ", function(){

        expect(typeof PicoScopeJS).toBe("function")
    })
    it("should be a ctor function", function(){

        expect(typeof (ps)).toBe("object")
    })
    // it("should have a template index", function(){

    //     expect(ps.templateIndex).toBeGreaterThanOrEqual(0);
    // })



    describe("PS2000er connected", function(){

        afterAll(function(){
            // ps.disconnect()
        })

        it("should connect to device", function(done){

            // expect((ps.connect())).toBeGreaterThan(0);
            ps.connect((...args: any[])=>{
                if(args[0] && args[0].deviceHandle > 0){
                    done()
                }
            })
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