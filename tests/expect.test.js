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

    // Boolean - Related
    test("toBeBoolean()", () => {
        expect(true).toBeBoolean();
        expect(false).toBeBoolean();
        expect(1).not.toBeBoolean();
        expect("Hello, Senkora!").not.toBeBoolean();
        expect({ a: 1, b: 2, c: 3 }).not.toBeBoolean();
        expect([1, 2, 3]).not.toBeBoolean();
        expect(null).not.toBeBoolean();
        expect(undefined).not.toBeBoolean();
    });

    test("toBeTrue()", () => {
        expect(true).toBeTrue();
        expect(false).not.toBeTrue();
        expect(1).not.toBeTrue();
        expect("Hello, Senkora!").not.toBeTrue();
        expect({ a: 1, b: 2, c: 3 }).not.toBeTrue();
        expect([1, 2, 3]).not.toBeTrue();
        expect(null).not.toBeTrue();
        expect(undefined).not.toBeTrue();
    });

    test("toBeFalse()", () => {
        expect(false).toBeFalse();
        expect(true).not.toBeFalse();
        expect(1).not.toBeFalse();
        expect("Hello, Senkora!").not.toBeFalse();
        expect({ a: 1, b: 2, c: 3 }).not.toBeFalse();
        expect([1, 2, 3]).not.toBeFalse();
        expect(null).not.toBeFalse();
        expect(undefined).not.toBeFalse();
    });

    // Array - Related
    test("toBeArray()", () => {
        expect([1, 2, 3]).toBeArray();
        expect(new Array(1, 2, 3)).toBeArray();
        expect([]).toBeArray();
        expect({ a: 1, b: 2, c: 3 }).not.toBeArray();
        expect(1).not.toBeArray();
        expect("Hello, Senkora!").not.toBeArray();
        expect(true).not.toBeArray();
        expect(false).not.toBeArray();
        expect(null).not.toBeArray();
        expect(undefined).not.toBeArray();
    });
    
    test("toBeArrayOfSize()", () => {
        expect([1, 2, 3]).toBeArrayOfSize(3);
        expect(new Array(1, 2, 3)).toBeArrayOfSize(3);
        expect([1, 2, 3]).not.toBeArrayOfSize(4);
        expect(new Array(1, 2, 3)).not.toBeArrayOfSize(4);
        expect([]).toBeArrayOfSize(0);
        expect({ a: 1, b: 2, c: 3 }).not.toBeArrayOfSize(3);
        expect(1).not.toBeArrayOfSize(1);
        expect("Hello, Senkora!").not.toBeArrayOfSize(13);
        expect(true).not.toBeArrayOfSize(1);
        expect(false).not.toBeArrayOfSize(1);
        expect(null).not.toBeArrayOfSize(1);
        expect(undefined).not.toBeArrayOfSize(1);
    });
});
