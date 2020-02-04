import PicoScopeJS from "../index";

describe("Picoscope napi implementation", function(){
    const ps = new PicoScopeJS()
    it("should be a function ", function(){

        expect(typeof PicoScopeJS).toBe("function")
    })
    it("should be a ctor function", function(){

        expect(typeof (ps)).toBe("object")
    })
    it("should have a template index", function(){

        expect(ps.templateIndex).toBeGreaterThanOrEqual(0);
    })



    describe("PS2000er connected", function(){

        afterAll(function(){
            // ps.disconnect()
        })

        it("should connect to device", function(){

            expect((ps.connect())).toBeGreaterThan(0);
        })
        
        
        it("should setChannelA", function(){
            
            expect(ps.setChannelA(true, true, 2)).toBeGreaterThan(0)
        })
        it("should setChannelB", function(){
            
            expect(ps.setChannelB(true, true, 2)).toBeGreaterThan(0)
        })

        it("should setTrigger", function(){

            expect(ps.setTrigger(0, 1, 1, 1, 1)).toBeGreaterThan(0)
        })
        let int16Max : Int16Array;
        it("should start reading", function(done){

            // expect.assertions.(2);
            expect(ps.stream(function(){
                console.log(arguments);
                // console.log("should start reading iteration", counter + 1)
                expect(arguments[0]).not.toBe(0);
                int16Max = new Int16Array((arguments[0].maxA as ArrayBuffer));

                done()
            })).not.toBe(0)
        }, 20000 )
        it("should disconnect", function(){

            expect(ps.disconnect()).toBe(true);
        })
        let timeout = 1000 * 30
        it("should be able to access int16Max", function(done){

            expect(int16Max.length).toBeGreaterThan(0);
            setTimeout(()=>{

                expect(typeof int16Max[100]).toBe("number")
                done()
            }, timeout)
        }, timeout)
    })
})