#!/bin/sh

curl "https://clickhouse-test-reports.s3.yandex.net/6781/aa10bc99ccd0e4e48abc816145cf4a9f0906ddaf/functional_stateless_tests_(release)/prepare_test_run.txt.out.txt" -o tests.out
hdfs dfs -mkdir text
hdfs dfs -mkdir text/full
hdfs dfs -put tests.out text/full/tests.out
head -n50 tests.out > tests.out.head
hdfs dfs -mkdir text/sample
hdfs dfs -put tests.out.head text/sample/tests.out

