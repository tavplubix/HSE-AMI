#!/bin/sh

wget -qO- "http://hadoop2-10.yandex.ru:50070/webhdfs/v1$1?op=OPEN&length=10"

