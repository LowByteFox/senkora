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

        const [ret, err] = Senkora.peekaboo(prom);
        expect([ret, err]).toEqual([undefined, new SenkoraError("Promise is not Fulfilled")]);
    });
});
