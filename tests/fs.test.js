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
