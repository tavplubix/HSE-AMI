#!/bin/sh

host=$(hdfs fsck -blockId $1 2>/dev/null | grep -F "Block replica on" | head -1 | grep -oE "hadoop.*\.yandex\.ru")
echo $host
ssh -i ~/.ssh/hse-hadoop atokmakov@$host "find / -name $1" 2>/dev/null

