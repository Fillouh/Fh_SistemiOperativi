#!/bin/sh
echo QUESTA E\' LA PROVA DEL 19 GIUGNO 2013
echo FILIPPO LEONELLI `date`

#controllo sul numero dei parametri
if test $# -ne 3
	then echo NUMERO DEI PARAMETRI SBAGLIATO
         exit 1
fi

#controllo sul tipo dei parametri
G=$1
K=$2
Y=$3

case $G in
	/*) if test -d $G -a -x $G
         then echo PARAMETRO 1 OK
         else
           echo PARAMETRO 1 NOT OK
        exit 2
    fi
        ;;
     *) echo PARAMETRO 1 NOT OK
        exit 2
        ;;
esac


test $K -ge 2 >/dev/null 2>&1
case $? in
	0);;
     *) echo PARAMETRO 2 NOT OK
        exit 4
        ;;
esac

test $Y -ge 0 >/dev/null 2>&1
case $? in
	0);;
     *) echo PARAMETRO 3 NOT OK
        exit 4
        ;;
esac

#impostazioni per la chiamata ricorsiva
PATH=`pwd`:$PATH
export PATH
> /tmp/dir$$

#chiamata ricorsiva
FCR.sh $G $K $Y /tmp/dir$$

#comandi post chiamata ricorsiva
for i in `cat /tmp/dir$$`
do
	cd $i
	for o in *
	do 
	echo FILE TROVATO: $o
    done
done
#rimozione file temporaneo
rm /tmp/dir$$

echo HO FINITO TUTTO
