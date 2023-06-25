import { writeToFile, readFromFile, exists, existsFile, existsDirectory, deleteFile, deleteDirectory, createDirectory } from "senkora:fs";
import { expect, describe, test } from "senkora:test";

const str = "Hello, Senkora!";
const date = new Date().toISOString().replaceAll(":", "_");
// TODO: Delete the file after each test.
const fname = "/tmp/senkora_test-" + date + ".txt";

describe('fs', () => {
    // TODO: Test if the file exists.
    test("writeToFile()", () => {
        writeToFile(fname, str)
        expect(existsFile(fname)).toBeTrue();
    });

    test("createDirectory()", () => {
        createDirectory("/tmp/senkora_test-" + date);
        expect(exists("/tmp/senkora_test-" + date)).toBeTrue();
    });

    test("readFromFile()", () => {
        const data = readFromFile(fname);
        expect(data).toEqual(str);
    });

    test("exists()", () => {
        expect(exists(fname)).toBeTrue();
        expect(exists("/tmp")).toBeTrue();
        expect(exists(fname + "a")).toBeFalse();
        expect(exists("/smthn")).toBeFalse();
    });

    test("existsFile()", () => {
        expect(existsFile(fname)).toBeTrue();
        expect(exists(fname + "a")).toBeFalse();
        expect(existsFile("/tmp")).toBeFalse(); // Directory
    });

    test("existsDirectory()", () => {
        expect(existsDirectory("/tmp")).toBeTrue();
        expect(existsDirectory(fname)).toBeFalse();
    });

    test("deleteFile()", () => {
        writeToFile(fname + "b", "Senkora")
        expect(existsFile(fname + "b")).toBeTrue();
        deleteFile(fname + "b");
        expect(existsFile(fname + "b")).toBeFalse();
    });

    test("deleteDirectory()", () => {
        expect(existsDirectory("/tmp/senkora_test-" + date)).toBeTrue();
        deleteDirectory("/tmp/senkora_test-" + date);
        expect(existsDirectory("/tmp/senkora_test-" + date)).toBeFalse();
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

    test("deleteDirectory() - Recursive", () => {
        const tmp = "/tmp/senkora_test-" + date;
        createDirectory(tmp);
        expect(existsDirectory(tmp)).toBeTrue();
        writeToFile(tmp + "/test.txt", "Senkora");
        expect(existsFile(tmp + "/test.txt")).toBeTrue();
        deleteDirectory(tmp, true);
        expect(existsDirectory(tmp)).toBeFalse();
        expect(existsFile(tmp + "/test.txt")).toBeFalse();
    });
});

describe("Cleanup", () => {
    test("Delete " + fname, () => {
        deleteFile(fname);
        expect(exists(fname)).toBeFalse();
    });
});