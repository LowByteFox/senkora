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

        expect({ a: 1, b: 2, c: 3, d: undefined }).toEqual({ a: 1, b: 2, c: 3, d: null });
        expect({ a: 1, b: 2, c: 3, d: undefined }).not.toEqual({ a: 1, b: 2, c: 4, d: null });

        expect(null).toEqual(null);
        expect(null).toEqual(undefined);

        expect(undefined).toEqual(undefined);
        expect(undefined).toEqual(null);

        expect(true).toEqual(true);
        expect(true).not.toEqual(false);
    });  

    test("toStrictEqual()", () => {
        expect("Hello, Senkora!").toStrictEqual("Hello, Senkora!");
        expect("Hello, Senkora!").not.toStrictEqual("Hello, Node!");

        expect(1).toStrictEqual(1);
        expect(1).not.toStrictEqual(2);

        expect(1.5).toStrictEqual(1.5);
        expect(1.5).not.toStrictEqual(2.5);

        expect([1, 2, 3]).toStrictEqual([1, 2, 3]);
        expect([1, 2, 3]).not.toStrictEqual([1, 2, 4]);

        expect({ a: 1, b: 2, c: 3, d: undefined }).toStrictEqual({ a: 1, b: 2, c: 3, d: undefined });
        expect({ a: 1, b: 2, c: 3, d: undefined }).not.toStrictEqual({ a: 1, b: 2, c: 3, d: null });

        expect(null).toStrictEqual(null);
        expect(null).not.toStrictEqual(undefined);

        expect(undefined).toStrictEqual(undefined);
        expect(undefined).not.toStrictEqual(null);

        expect(true).toStrictEqual(true);
        expect(true).not.toStrictEqual(false);
    });
    
    test("toBeEmpty()", () => {
        expect("").toBeEmpty();
        expect([]).toBeEmpty();
        expect({}).toBeEmpty();
        expect(new Map()).toBeEmpty();
        expect(new Object()).toBeEmpty();
        expect(null).toBeEmpty();
        expect(undefined).toBeEmpty();
        expect(0).not.toBeEmpty();
        expect(1).not.toBeEmpty();
        expect("Hello, Senkora!").not.toBeEmpty();
        expect([1, 2, 3]).not.toBeEmpty();
        expect({ a: 1, b: 2, c: 3 }).not.toBeEmpty();
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

    // Object - Related
    test("toBeObject()", () => {
        expect({ a: 1, b: 2, c: 3 }).toBeObject();
        expect(new Object()).toBeObject();
        expect({}).toBeObject();
        expect([1, 2, 3]).not.toBeObject();
        expect(1).not.toBeObject();
        expect("Hello, Senkora!").not.toBeObject();
        expect(true).not.toBeObject();
        expect(false).not.toBeObject();
        expect(null).not.toBeObject();
        expect(undefined).not.toBeObject();
    });

    test("toBeOneOf()", () => {
        expect(1).toBeOneOf([1, 2, 3]);
        expect(2).toBeOneOf([1, 2, 3]);
        expect(3).toBeOneOf([1, 2, 3]);
        expect(4).not.toBeOneOf([1, 2, 3]);
        expect(5).not.toBeOneOf([1, 2, 3]);
        expect(6).not.toBeOneOf([1, 2, 3]);
        expect("Hello, Senkora!").not.toBeOneOf([1, 2, 3]);
        expect(true).not.toBeOneOf([1, 2, 3]);
        expect(false).not.toBeOneOf([1, 2, 3]);
        expect(null).not.toBeOneOf([1, 2, 3]);
        expect(undefined).not.toBeOneOf([1, 2, 3]);
    });
});

// .then works
describe("expect() - Advanced", () => {
    // TODO: Fix it being printed out as Async #2 too (see below)
    test("Async #0", async () => {
        expect(true).toBeTrue();
    });

    // TODO: Fix it printing Asnyc #2 and Async #2
    test("Async #1", async () => {
        const todos = await getBack([1]);
        expect(todos).toEqual([1]);
    });

    test("Async #2", async () => {
        const todos = await getBack(2);
        expect(todos).toEqual(2);
    });

    // TODO?: Fix it being printing before Async #2
    test("Normal after Async", () => {
        expect(true).toBeTrue();
    });
});

function getBack(arg) {
    return new Promise((resolve, reject) => {
        setTimeout(() => {
            resolve(arg);
        }, 500);
    });
}
