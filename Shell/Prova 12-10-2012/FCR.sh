#!/bin/sh

cd $1

nf=0 #variabile per memorizzare il numero di file che soddisfano i vincoli
trovato=0

for i in *
do
	trovato=0
	if test -f $i -a -r $i
	then
		if grep $2 $i >/dev/null 2>&1 
		then
			nf=`expr $nf + 1`
			trovato=1
			
		fi
		if grep $3 $i >/dev/null 2>&1
		then
			nf=`expr $nf + 1`
			trovato=1
			
		fi
	fi
	if test $trovato -eq 1
	then
		echo `pwd`/$i >> $4
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
		FCR.sh `pwd`/$i $2 $3 $4
	fi
done
