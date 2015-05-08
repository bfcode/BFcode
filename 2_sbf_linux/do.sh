#!/bin/sh
echo "Test begin"   #this script is used for testing

i=1
while [ "$i" -le 10 ]
do
	echo -e "========================\n./sbf rand_20b_100M.tr 20 $i" 
	./sbf rand_20b_100M.tr 20 $i
	i=$(($i+1))
done

i=10
while [ "$i" -le 100 ]
do
	echo -e "========================\n./sbf rand_20b_100M.tr 20 $i" 
	./sbf rand_20b_100M.tr 20 $i
	i=$(($i+10))
done
