#!/bin/sh
echo QUESTA E\' LA PROVA DEL 15 FEBBRAIO 2012
echo FILIPPO LEONELLI `date`

#controllo sul numero dei parametri
if test $# -ne 2
	then echo NUMERO DI PARAMETRI PASSATI NON CORRETTO
         exit 1
fi

#controllo sul tipo dei parametri
G=$1
H=$2

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

#impostazione per la chiamata ricorsiva
PATH=`pwd`:$PATH
export PATH
>/tmp/file

#chiamata al file ricorsivo
FCR.sh $G $H /tmp/file

#operazioni post chiamata al file ricorsivo

for i in `cat /tmp/file`
do
	echo FILE : $i
	echo CONTENUTO DEL FILE :
	echo `cat $i`
done

rm /tmp/file

echo HO FINITO TUTTO
