#!/bin/sh
echo QUESTA E\' LA PROVA DEL 22 GIUGNO 2011
echo FILIPPO LEONELLI `date`

#controllo del numero dei parametri
if test $# -ne 4
	then echo NUMERO DI PARAMETRI PASSATI NON CORRETTO
         exit 1
fi

#controllo del tipo dei parametri
G=$1
N=$2
H=$3
C=$4

#controllo del primo parametro
case $G in 
	/*)if test -d $G -a -x $G
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

#controllo del secondo parametro
test $N -ge 0 >/dev/null 2>&1
case $? in
	0)echo PARAMETRO 2 OK
      ;;
    *) echo PARAMETRO 2 NOT OK
       exit 4
        ;;
esac

#controllo del terzo parametro
test $H -ge 0 >/dev/null 2>&1
case $? in
	0)echo PARAMETRO 3 OK
      ;;
    *) echo PARAMETRO 3 NOT OK
       exit 5
        ;;
esac

#controllo del quarto parametro
case $C in
	?) echo PARAMETRO 4 OK
       ;;
    *) echo PARAMETRO 3 NOT OK
       exit 5
        ;;
esac

#impostazione per la chiamata ricorsiva
PATH=`pwd`:$PATH
export PATH

>/tmp/file

FCR.sh $G $N $H $C /tmp/file

#post chiamata ricorsiva
for i in `cat /tmp/file`
do
	echo FILE : $i
done

rm /tmp/file

echo HO FINITO TUTTO
