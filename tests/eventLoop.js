let i = 1000;

function test() {
    i -= 1;
    println(`Function with iteration ${i}`);
    if (i > 0) {
        setTimeout(test, i);
    }
}

test();
