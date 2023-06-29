/*
Senkora - JS runtime for the modern age
Copyright (C) 2023  SenkoraJS

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
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

    test("toBeOneOf()", () => {
        expect(1).toBeOneOf([4, 2, 3]);
    });

    test("not.toBeOneOf()", () => {
        expect(1).not.toBeOneOf([1, 3, 4]);
    });

    // Async - Related
    test("Async toEqual()", async () => {
        const result = await getBack("Hello, Senkora!");
        expect(result).toEqual("Hello, Node!");
    });
});

function getBack(arg) {
    return new Promise((resolve, reject) => {
        setTimeout(() => {
            resolve(arg);
        }, 500);
    });
}
