#!/bin/sh
echo QUESTA E\' LA PROVA DEL 14 SETTEMBRE 2011
echo FILIPPO LEONELLI `date`

#controllo del numero dei parametri
if test $# -ne 3
then 
	echo NUMERO DI PARAMETRI PASSATI NON CORRETTO
	exit 1
fi

#controllo del tipo dei parametri
G=$1
H=$2
K=$3

#controllo sul primo parametro
case $G in
	/*) if test -d $G -a -x $G
        then echo PARAMETRO 1 OK
    else
    	echo PARAMETRO 1 NOT OK
    	exit 2
    fi
    ;;
    *) echo PARAMETRO 1 NOT OK
    	exit 3
    	;;
esac

#controllo del secondo parametro e del terzo parametro
test $K -ge 0 >/dev/null 2>&1
case $? in 
	0) echo PARAMETRO 3 OK
       ;;
    *)echo PARAMETRO 3 NOT OK
    	exit 4
    	;;
esac

test $H -ge 0 >/dev/null 2>&1
case $? in 
	0) if test $H -lt $K 
       then 
       	 echo PARAMETRO 2 OK
       	else
       		echo PARAMETRO 2 NOT OK
       		exit 5
       	fi
       ;;
    *)echo PARAMETRO 3 NOT OK
    	exit 6
    	;;
esac

#impostazione per la chiamata ricorsiva
PATH=`pwd`:$PATH
export PATH
>/tmp/file

FCR.sh $G $H $K /tmp/file

for i in `cat /tmp/file`
do
	echo FILE : $i
done

rm /tmp/file

echo HO FINITO TUTTO
