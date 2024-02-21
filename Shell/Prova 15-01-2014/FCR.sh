#!/bin/sh
cd $1
nf=0 #variabile contatore file che soddisfano i vincoli
l=0 #variabile per la lunghezza in byte dei file
for i in *
do
  if test -f $i
  then 
  	l=`wc -c < $i`
  	if test $l -gt $2
  	then
  		nf=`expr $nf + 1`
  		echo `pwd`/$i >> $3
  	fi
  fi
done

if test $nf -ge 1
then 
	echo DIRECTORY SODDISFACENTE : `pwd`
fi

for i in *
do
	if test -d $i -a -x $i
	then
		FCR.sh `pwd`/$i $2 $3
	fi
done
