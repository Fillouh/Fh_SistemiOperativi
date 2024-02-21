#!/bin/sh

cd $1
nf=0 #variabile di conteggio per i file
for i in *
do
   if test -f $i -a -r $i
   then
   	   nf=`expr $nf + 1`
   fi
done

if test $nf -ge $2 -a $nf -le $3
then
	echo DIRECTORY CHE SODDISFA I VINCOLI : `pwd`
    for i in *
    do
    	if test -f $i -a -r $i
    	then
    		echo `pwd`/$i >> $4
    	fi
    done
fi

for i in *
do
	if test -d $i -a -x $i
	then
		FCR.sh `pwd`/$i $2 $3 $4
	fi
done
