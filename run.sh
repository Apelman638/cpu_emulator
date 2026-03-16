#!/bin/bash
python3 assembler.py 
g++ main.cpp gpu_emulator.cpp -o simulator
./simulator