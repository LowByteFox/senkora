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
const sleep = (ms) => new Promise((resolve) => setTimeout(resolve, ms));

async function test() {
    println("Hello");
    await sleep(1000);
    println("Goodbye");
}

await test();

setTimeout(async () => {
    await sleep(500);
    println("Hello");
}, 10);
