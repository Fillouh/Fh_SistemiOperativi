#!/bin/sh
echo QUESTA E\' LA PROVA DEL 12 OTTOBRE DEL 2012
echo FILIPPO LEONELLI `date`

#controllo sul numero dei parametri
if test $# -ne 3
	then echo NUMERO DEI PARAMETRI PASSATI NON CORRETTO
         exit 1
fi

#controllo sul tipo dei parametri
G=$1
C1=$2
C2=$3

#controllo sul primo parametro
case $G in
	/*) if test -d $G -a -x $G
        then
        	 echo PARAMETRO 1 OK
        else echo PARAMETRO 1 NOT OK
             exit 2
        fi
        ;;
     *) echo PARAMETRO 1 NOT OK
        exit 3
        ;;
esac

#controllo sul secondo parametro
case $C1 in
	?) echo PARAMETRO 2 OK
       ;;
    *) echo PARAMETRO 2 NOT OK
       exit 4
       ;;
esac

#controllo sul terzo parametro
case $C2 in
	?) echo PARAMETRO 3 OK
       ;;
    *) echo PARAMETRO 3 NOT OK
       exit 5
       ;;
esac


#impostazione per la chiamata al file ricorsivo
PATH=`pwd`:$PATH
export PATH

>/tmp/file

#chiamata al file ricorsivo
FCR.sh $G $C1 $C2 /tmp/file

#comandi post file ricorsivo
for i in `cat /tmp/file`
do
	echo FILE : $i
done

rm /tmp/file

echo HO FINITO TUTTO
