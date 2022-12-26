#!/bin/sh

hdfs fsck $1 -files -blocks -locations 2>/dev/null | grep -F "[DatanodeInfoWithStorage[" | head -n1 | grep -oE "\[DatanodeInfoWithStorage\[(([0-9]{1,3}\.){3}[0-9]{1,3})\b" | grep -oE "([0-9]{1,3}\.){3}[0-9]{1,3}"

