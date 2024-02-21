#!/bin/sh

cd $1
nr=0 #variabile per misurare il criterio

for i in *
do
	if test -f $i -a -r $i
	then
		nr=`grep 't$' $i | wc -l `
		if test $nr -ge $2
		then
			echo `pwd`/$i >>$3
		fi
	fi
done

for i in *
do
	if test -d $i -a -x $i
	then
		FCR.sh `pwd`/$i $2 $3
	fi
done
