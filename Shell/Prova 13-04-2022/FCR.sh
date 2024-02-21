#!/bin/sh

cd $2

for i in *
do
	if test -f $i -a -r $i
	then
		nl=`wc -l < $i`
		if test $nl -eq $1
		then
			echo `pwd`/$i >> $3
		fi
	fi
done

for i in *
do
	if test -d $i -a -x $i
	then
		FCR.sh $1 `pwd`/$i $3
	fi
done
