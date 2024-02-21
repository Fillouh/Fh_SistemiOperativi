#!/bin/sh
echo QUESTA E\' LA PROVA DEL 13 LUGLIO 2011
echo FILIPPO LEONELLI `date`

#controllo sul numero dei parametri
if test $# -ne 3
then
	echo NUMERO DEI PARAMETRI PASSATI NON CORRETTO
	exit 1
fi

#controllo sul tipo dei parametri
G=$1
H=$2
C=$3

#controllo sul primo parametro
case $G in
	/*) if test -d $G -a -x $G
        then
        	echo PARAMETRO 1 OK
        else
        	echo PARAMETRO 1 NOT OK
        	exit 2
        fi
        ;;
      *) echo PARAMETRO 1 NOT OK
        	exit 3
        ;;
esac

#controllo sul secondo parametro
test $H -ge 0 >/dev/null 2>&1
case $? in
	0) echo PARAMETRO 2 OK
       ;;
     *) echo PARAMETRO 2 NOT OK
        exit 4
       ;;
esac

#controllo sul terzo parametro
case $C in
	?) echo PARAMETRO 3 OK
       ;;
     *) echo PARAMETRO 3 NOT OK
        exit 5
        ;;
esac

#impostazioni per la chiamata ricorsiva
PATH=`pwd`:$PATH
export PATH
>/tmp/file

#chiamata ricorsiva
FCR.sh $G $H $C /tmp/file

#comandi post chiamata ricorsiva
for i in `cat /tmp/file`
do
	echo FILE : $i
done

rm /tmp/file

echo HO FINITO TUTTO
