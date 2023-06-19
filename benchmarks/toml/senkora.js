import { readFromFile } from "senkora:fs";
import { parse } from "senkora:toml";

const content = readFromFile("test.toml");
println(parse(content));