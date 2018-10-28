#!/bin/bash

cd post-to-html/
gcc -shared -fPIC post-to-html.c -o post-to-html.o
cd ..
