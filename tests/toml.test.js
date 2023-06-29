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
import { readFromFile } from "senkora:fs";
import { parse } from "senkora:toml";

const content = readFromFile("test.toml");

describe("toml", () => {
    test("parse", () => {
        const parsed = parse(content);

        expect(parsed).toEqual({
            "ahoj": "cau",
            "config": {
                "daco": [{ "a": 1, "b": 2 }, { "a": 3, "b": 4 }],
                "nested": {
                    "something": [[1, 2], [3, 4]]
                },
                "something": "ahoj"
            }
        });
    })
});
