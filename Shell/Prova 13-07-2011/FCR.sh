#!/bin/sh

cd $1
nf=0 #variabile per contare i file
nl=0 #variabile per contare le righe del file
occ=
for i in *
do
	if test -f $i -a -r $i
	then
		nl=`wc -c <$i`
		if test $nl -le $2
		then
			occ=`grep $3 $i | wc -l`
			if test $occ -ge 1
			then
				nf=`expr $nf + 1`
			fi
		fi
	fi
done

if test $nf -ge 1
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
