let i = 500;
let j = 0;

function test() {
    i -= 1;
    println(`Function with iteration ${i}`);
    if (i > 0) {
        let id = setTimeout(test, i);
        if (id > 10) {
            clearTimeout(id);
            immediateTest();
        }
    }
}

function immediateTest() {
    println(`Immediate function with iteration ${j}`);
    let id = setImmediate(immediateTest);
    j += 1;

    if (j > 10) {
        clearImmediate(id);
    }
}

setTimeout(test, i);
