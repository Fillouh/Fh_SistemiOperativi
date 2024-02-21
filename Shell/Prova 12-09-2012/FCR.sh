#!/bin/sh

cd $1
nf=0 #variabile per contare i file che rispettano il vincolo

for i in *
do
	if test -f $i -a -r $i
	then
		if grep -i $2 $i >/dev/null 2>&1
		then
			echo STO SOMMANDO NF
			nf=`expr $nf + 1`
			echo `pwd`/$i >> $3
		fi
	fi
done

if test $nf -ge 1
then
	echo DIRECTORY CHE SODDISFA I VINCOLI : `pwd`
fi

for i in *
do
	if test -d $i -a -x $i
	then
		FCR.sh `pwd`/$i $2 $3
	fi
done
