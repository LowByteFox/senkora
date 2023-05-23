import "./data.js";

const console = {
    log: __PRIVATE_CUZ_FF_STUPID.print
};

console.log(5);
console.log(7.8);
console.log(null);
console.log(undefined);


function ahoj() {
    return 5;
}

class Xd {
    constructor() {
        this.hello = "ano";
    }
}

console.log(ahoj)

console.log({
    bun: "Epic",
    node: "Old",
    deno: "Ruined",
    Senkora: "Spicy?",
    ahoj,
    h: {
        jeToTak: true,
        hodnota: 5,
        dalsieCislo: 7.8,
        nic: null,
        neznam :undefined,
        trieda: {
            Xd: Xd
        }
    },
    ahoj2: "cau"
});

console.log(Xd);

const { url } = import.meta;

console.log(import.meta);
console.log(url);
