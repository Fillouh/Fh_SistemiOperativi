#!/bin/sh

cd $1
nf=0 #variabile per contare i file che rispettano i vincoli specificati
nl=0 #variabile per contare le righe del file corrente

for F in *
do
	if test -f $F -a -r $F
	then
		nl=`wc -l <$F`
		if test $nl -eq $2
		then
			nf=`expr $nf + 1`
			echo `pwd`/$F >> $3
		fi
	fi
done

if test $nf -ge 2
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
