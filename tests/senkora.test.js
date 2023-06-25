import { expect, describe, test } from "senkora:test";

describe("Senkora", () => {
    test("version", () => {
        expect(Senkora.version).toEqual("0.0.1");
    });
    test("peekaboo", () => {
        async function hey() {
            return Promise.resolve("kon kon");
        }

        const prom = hey();
        setImmediate(() => {
            // at this point the promise is fulfilled
            // but the object is still Promise 
            // and you don't want to make the function async
            println(Senkora.peekaboo(prom));
        });
    });
});
