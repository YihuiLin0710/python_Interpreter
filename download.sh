#!/bin/bash

for i in {10..17}; do curl -O `printf "https://www2.cs.uh.edu/~dss/teaching/COSC4315/testcases/pyinterpreter-expressions/phase_2/in%d.py" $i `; done