import { expect, describe, test } from "senkora:test";

describe("Senkora", () => {
    test("version", () => {
        expect(Senkora.version).toEqual("0.0.1");
    });
});