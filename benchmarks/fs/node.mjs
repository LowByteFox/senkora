import { writeFileSync, readFileSync } from "node:fs";

const str = "Hello World!";

[1, 10, 100, 1000, 10000].forEach((i) => {
    writeFileSync("test.txt", str.repeat(i));
    const content = readFileSync("test.txt");
});