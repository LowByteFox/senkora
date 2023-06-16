import { writeToFile, readFromFile } from "senkora:fs";

const str = "Hello World!";

[1, 10, 100, 1000, 10000].forEach((i) => {
    writeToFile("test.txt", str.repeat(i));
    const content = readFromFile("test.txt");
});