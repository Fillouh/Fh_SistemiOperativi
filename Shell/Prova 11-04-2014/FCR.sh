#!/bin/sh

cd $1

nl=0 #variabile che mi serve per contare le righe totali del file
ns=0 #variabile che mi serve per contare le righe che soddisfano il vincolo

for F in *
do
	if test -f $F -a -r $F
	then
		nl=`wc -l < $F`
		if test $nl -eq 0
		then
			continue
		fi
		nr=`grep '^a' $F | wc -l`
		if test $nr -eq $nl
		then
			echo HO TROVATO UN FILE CHE SODDISFA I VINCOLI
			echo `pwd`/$F >> $2
		fi
	fi
done

for i in *
do
	if test -d $i -a -x $i
	then
		FCR.sh `pwd`/$i $2
	fi
done
