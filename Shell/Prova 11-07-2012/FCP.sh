#!/bin/sh
echo QUESTA E\' LA PROVA DELL\' 11 LUGLIO 2012
echo FILIPPO LEONELLI `date`

#controllo numero dei parametri
if test $# -ne 2
	then echo NUMERO DEI PARAMETRI PASSATI ERRATO
         exit 1
fi

#controllo sul tipo dei parametri
G=$1
N=$2

case $G in 
	/*);;
     *) echo PARAMETRO 1 NOT OK
        exit 2
        ;;
esac

if test ! -d $G -a ! -x $G
then 
	echo PARAMETRO 1 NOT OK
        exit 3
fi

test $N -ge 2 >/dev/null 2>&1
case $? in
	0);;
     
     *) echo PARAMETRO 2 NOT OK
        exit 4
        ;;
esac

PATH=`pwd`:$PATH
export PATH

>/tmp/dir$$

FCR.sh $G $N /tmp/dir$$

#post chiamata ricorsiva

for i in `cat /tmp/dir$$`
do
	cd $i
	for o in *
	do 
		C=`wc -c < $o`
		H=`expr $C / $N`
		echo FILE IN `pwd` : $o $H
	done
done

#rimuovo il file temporaneo

echo HO FINITO TUTTO
