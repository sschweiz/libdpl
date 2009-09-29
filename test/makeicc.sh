#!/bin/bash
OMPI_CC=/opt/intel/cc/9.1.043/bin/icc mpicc -openmp cluster.c -o cluster -ldpl -DDPL_OPENMP
