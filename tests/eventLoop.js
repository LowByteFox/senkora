let i = 500;

function test() {
    i -= 1;
    println(`Function with iteration ${i}`);
    if (i > 0) {
        setTimeout(test, i);
    }
}

setTimeout(test, i);
