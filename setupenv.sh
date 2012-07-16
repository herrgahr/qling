#!/bin/sh
#source this file to setup the environment for building and running qling

#change to match your install
export LLVM_INSTALL=/home/thomas/opt/llvm-debug


export LD_LIBRARY_PATH=${LLVM_INSTALL}/lib
export PATH=${LLVM_INSTALL}/bin:$PATH
