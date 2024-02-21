#!/bin/sh

cd $1
trovato=0
for i in *
do
	if test -d $i
	then
		trovato=0
		break
	else
		nl=`wc -l <$i`
		if test $nl -le $2
		then
			trovato=0
			break
		else
            trovato=1
        fi
    fi
done

if test $trovato -eq 1 
then
	pwd >> $3
fi

for i in *
do
	if test -d $i -a -x $i
	then
		FCR.sh `pwd`/$i $2 $3
	fi
done
