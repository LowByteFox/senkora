#!/bin/bash

for file in ./tests/*.test.js
do
    if [[ $file == *"errors.test.js"* ]]; then
      continue
    fi
    echo "Running $file"
    ./dist/senkora run $file
done