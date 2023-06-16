function crash() {
    throw new AggregateError([new Error("World"), new AggregateError([new Error("World2")], "msg2")], "msg");
}

crash();