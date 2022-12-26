#!/bin/bash

dd if=/dev/zero of=test_file.tmp bs=$1 count=1 &> /dev/null
hdfs dfs -put test_file.tmp /user/atokmakov/test_file.tmp 
rm test_file.tmp
blocks=($(hdfs fsck /user/atokmakov/test_file.tmp -blocks -files 2>/dev/null | grep -oE "\bblk_[0-9]+"))

sum=0
for block in "${blocks[@]}"
do
	#echo "Block $block"
	host=$(hdfs fsck -blockId $block 2>/dev/null | grep -F "Block replica on" | head -1 | grep -oE "hadoop.*\.yandex\.ru")
	#echo "Host $host"
	path=$(ssh -i ~/.ssh/hse-hadoop atokmakov@$host "find / -name $block" 2>/dev/null)
	#echo "Path $path"
	size=$(ssh -i ~/.ssh/hse-hadoop atokmakov@$host "du -B1 $path | grep -Eo "[0-9]+" | head -1")
	#echo "Size $size"
	sum=`expr $sum + $size`
done

#echo "Sum $sum"
echo "$(($sum - $1))"

hdfs dfs -rm /user/atokmakov/test_file.tmp &> /dev/null

