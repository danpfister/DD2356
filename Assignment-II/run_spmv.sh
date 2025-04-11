#!/bin/bash

for arg in 10 100 1000 10000; do
    ./spmv.out "$arg"
done