#!/bin/sh

cd $1
nl=0 #variabile per contare il numero di righe del file corrente

for i in *
do
	if test -f $i -a -r $i
	then
		nl=`wc -l < $i`
		if test $nl -ge $2
		then
			if test $nl -ge 5
			then
				riga=`head -5 $i | tail -1`
				> $i.quinta
				 echo $riga >> $i.quinta
				 echo `pwd`/$i.quinta >> $3
			else
				>$i.NOquinta
				echo `pwd`/$i.NOquinta >> $3
			fi
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
