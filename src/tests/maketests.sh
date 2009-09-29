#!/bin/bash

gcc -O2 -ggdb listtest.c ../dpl_list.c ../dpl_error.c ../dpl_base.c -o listtest
gcc -O2 -ggdb declaretest.c -o declaretest
gcc -O2 -ggdb stacktest.c ../dpl_stack.c ../dpl_error.c ../dpl_base.c -o stacktest 
gcc -O2 -ggdb fifotest.c -o fifotest
