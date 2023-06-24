import { describe, expect, test } from "senkora:test";

describe("JSON", () => {
    test("parse()", () => {
        const parsed = JSON.parse('{"ahoj": "cau"}');

        expect(parsed).toEqual({ ahoj: "cau" });
        expect(parsed.ahoj).toEqual("cau");
        expect(parsed["ahoj"]).toEqual("cau");

        expect(parsed).not.toEqual({ ahoj: "caus" });
    });

    test("parse() & Reviver", () => {
        const parsed = JSON.parse('{"ahoj": "cau", "ay": "sup"}', (key, value) => {
            if (key === "ahoj") {
                return "Hello";
            }

            return value;
        });

        expect(parsed).toEqual({ ahoj: "Hello", ay: "sup" });
        expect(parsed.ahoj).toEqual("Hello");
        expect(parsed["ahoj"]).toEqual("Hello");

        expect(parsed).not.toEqual({ ahoj: "caus" });
    });

    test("stringify()", () => {
        const stringified = JSON.stringify({ ahoj: "cau" });
        expect(stringified).toEqual('{"ahoj":"cau"}');
        expect(stringified).not.toEqual('{\n    "ahoj": "cau"\n}');
    });

    test("stringify() & Replacer", () => {
        const stringified = JSON.stringify({ ahoj: "cau" }, null, 4);
        expect(stringified).toEqual('{\n    "ahoj": "cau"\n}');
        expect(stringified).not.toEqual('{"ahoj":"cau"}');
    });
});

describe("JSON - Advanced", () => {
    test("deep parse()", () => {
        const parsed = JSON.parse('{"ahoj": "cau", "ay": "sup", "nested": {"something": "else", "idk": ["what", "b"] }}');

        expect(parsed).toEqual({ ahoj: "cau", ay: "sup", nested: { something: "else", idk: ["what", "b"] } });
        expect(parsed.ahoj).toEqual("cau");
        expect(parsed["ahoj"]).toEqual("cau");

        expect(parsed).not.toEqual({ ahoj: "caus" });
    });

    test("deep stringify()", () => {
        const stringified = JSON.stringify({
            ahoj: "cau",
            ay: "sup",
            nested: {
                something: "else",
                idk: ["what", "b"]
            }
        });

        expect(stringified).toEqual('{"ahoj":"cau","ay":"sup","nested":{"something":"else","idk":["what","b"]}}');
    });
});