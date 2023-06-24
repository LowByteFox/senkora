import fs, { writeToFile, readFromFile } from "senkora:fs";
import { expect, describe, test } from "senkora:test";


const str = "Hello, Senkora!";
const date = new Date().toISOString().replaceAll(":", "_");
// TODO: Delete the file after each test.
const fname = "/tmp/senkora_test-" + date + ".txt";

describe('fs', () => {
    // TODO: Test if the file exists.
    test("writeToFile()", () => {
        writeToFile(fname, str)
    });

    test("readFromFile()", () => {
        const data = readFromFile(fname);
        expect(data).toEqual(str);
    });
});

describe("fs - Advanced", () => {
    test("Large File", () => {
        for (const i of [1, 10, 100, 1000, 10000]) {
            writeToFile(fname, str.repeat(i));
            const data = readFromFile(fname);
            expect(data).toEqual(str.repeat(i));
        };
    });
});