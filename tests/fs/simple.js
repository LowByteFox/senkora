import * as fs from "senkora:fs";
import fss from "senkora:fs";
import { writeToFile, readFromFile } from "senkora:fs";

println(typeof fs.writeToFile !== "undefined");
println(typeof fss.writeToFile !== "undefined");
println(typeof writeToFile !== "undefined");
println(typeof readFromFile !== "undefined");
