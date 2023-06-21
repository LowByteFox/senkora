import { describe, test, expect } from "senkora:test";

describe("expect()", () => {
    test("toEqual()", () => {
        expect("Hello, Senkora!").toEqual("Hello, Senkora!");
        expect("Hello, Senkora!").not.toEqual("Hello, Node!");

        expect(1).toEqual(1);
        expect(1).not.toEqual(2);

        expect(1.5).toEqual(1.5);
        expect(1.5).not.toEqual(2.5);

        expect([1, 2, 3]).toEqual([1, 2, 3]);
        expect([1, 2, 3]).not.toEqual([1, 2, 4]);

        expect({ a: 1, b: 2, c: 3 }).toEqual({ a: 1, b: 2, c: 3 });
        expect({ a: 1, b: 2, c: 3 }).not.toEqual({ a: 1, b: 2, c: 4 });

        expect(null).toEqual(null);
        expect(null).not.toEqual(undefined);

        expect(undefined).toEqual(undefined);
        expect(undefined).not.toEqual(null);

        expect(true).toEqual(true);
        expect(true).not.toEqual(false);
    });  
});
