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
