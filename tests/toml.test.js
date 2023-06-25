import { expect, describe, test } from "senkora:test";
import { readFromFile } from "senkora:fs";
import { parse, xd } from "senkora:toml";

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
