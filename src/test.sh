#!/bin/bash

make
for ((i = 1; i <= 100; i++)); do
    seed=$(($RANDOM + i))
    ./main.out "$seed"
done
