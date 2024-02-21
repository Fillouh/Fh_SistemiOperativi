#!/bin/sh

cd $1
nf=0 #variabile per poter contare i file che soddisfano i vincoli
nl=0 #variabile per contare le linee del file corrente
occ=0 #variabile per confrontare il numero di occorrenze del carattere nel file

for i in *
do
	if test -f $i -a -r $i
	then
		nl=`wc -l < $i`
		if test $nl -eq $3
		then
			occ=`grep $4 $i | wc -l `
			if test $occ -ge 1
			then
				nf=`expr $nf + 1`
			fi
		fi
	fi
done

if test $nf -ge $2
then
	echo DIRECTORY CHE SODDISFA I VINCOLI : `pwd`
	for i in *
	do
		if test -f $i -a -r $i
		then
			echo `pwd`/$i >> $5
		fi
	done
fi

for i in *
do
	if test -d $i -a -x $i
	then
		FCR.sh `pwd`/$i $2 $3 $4 $5
	fi
done
