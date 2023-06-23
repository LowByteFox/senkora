import { describe, test, expect } from "senkora:test";

describe("expect()", () => {
    test("toEqual()", () => {
        expect("Hello, Senkora!").toEqual("Hello, Node!");
    });

    test("not.toEqual()", () => {
        expect("Hello, Senkora!").not.toEqual("Hello, Senkora!");
    });

    // Array - Related
    test("toBeArray()", () => {
        expect(1).toBeArray();
    });

    test("not.toBeArray()", () => {
        expect([]).not.toBeArray();
    });

    test("toBeArrayOfSize()", () => {
        expect([1, 2, 3]).toBeArrayOfSize(4);
    });

    test("toBeArrayOfSize()", () => {
        expect("Hello, Senkora!").toBeArrayOfSize(4);
    });

    test("not.toBeArrayOfSize()", () => {
        expect([1, 2, 3]).not.toBeArrayOfSize(3);
    });

    // Boolean - Related
    test("toBeBoolean()", () => {
        expect(1).toBeBoolean();
    });

    test("not.toBeBoolean()", () => {
        expect(true).not.toBeBoolean();
    });

    test("toBeTrue()", () => {
        expect(false).toBeTrue();
    });

    test("not.toBeTrue()", () => {
        expect(true).not.toBeTrue();
    });

    test("toBeFalse()", () => {
        expect(true).toBeFalse();
    });

    test("not.toBeFalse()", () => {
        expect(false).not.toBeFalse();
    });
});

describe("Adavanced", () => {
    // async function quickReturn() { return "Hello" }; <= This will work
    function notQuickReturn() { return new Promise(resolve => setTimeout(() => resolve("Hello!"), 100)) }; // <= Throw a warning

    // Should throw: "Warning: Test has a promise, this is not supported yet"
    test("Async Warning", async () => {
        expect(await notQuickReturn()).toEqual("hi!");
    });
});