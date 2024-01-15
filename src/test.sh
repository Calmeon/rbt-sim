#!/bin/bash

# Srcipt to perform many different simulations
# Purpose is to check for eventuall errors
# It is needed because of randomness of simulator  

make
for ((i = 1; i <= 100; i++)); do
    seed=$(($RANDOM + i))
    ./main.out "$seed"
done
