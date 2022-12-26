#!/usr/bin/env bash

rm res
rm log

./test.py 100000 10 1>res 2>log

rm fast.png
rm slow.png
rm both.png

head -4 res | ./plot.py fast.png
tail -4 res | ./plot.py slow.png
cat res | ./plot.py both.png
