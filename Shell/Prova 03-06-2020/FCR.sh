#!/bin/sh

cd $1
trovato=0 #variabile che mi impedisce di riscrivere sempre la stessa directory
for i in *
do
	if test -d $i -a -x $i
	then
		case $i in
			$2?$2) trovato=1
                   ;;
                *) ;;
        esac
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
