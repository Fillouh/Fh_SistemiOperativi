#!/bin/sh
cd $1
trovato=0 #variabile che mi serve ad identificare se sono in una directory senza altre directory
nf=0 #variabile che mi serve per contare i file nella directory corrente
for i in *
do 
	if test -d $i 
		then trovato=1
	         break
	else 
		 nf=`expr $nf + 1`
    fi
done

if test $trovato -eq 0
then 
	if test $nf -eq $2
	then
		echo TROVATA UNA DIRECTORY CHE SODDISFA LE CONDIZIONI : `pwd`
	    echo `pwd` >> $3
	fi
fi

for i in *
do 
	if test -d $i -a -x $i
	then
		FCR.sh `pwd`/$i $2 $3
	fi
done
