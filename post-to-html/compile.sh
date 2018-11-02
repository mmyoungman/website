#!/bin/bash

cd post-to-html/
#gcc -shared -fPIC post-to-html.c -D DEBUG -o post-to-html.o
gcc -shared -fPIC post-to-html.c -o post-to-html.o
cd ..
