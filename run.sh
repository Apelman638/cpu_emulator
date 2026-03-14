#!/bin/bash

python3 tokenizer.py test.asm
g++ components.cpp -o simulator
./simulator