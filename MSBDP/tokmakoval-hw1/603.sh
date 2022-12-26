#!/bin/sh

hdfs fsck $1 -blocks 2>/dev/null | grep -F "Total blocks" | grep -oE "[0-9]+" | head -1

