#!/bin/sh
echo QUESTA E\' LA PROVA DEL 20 GIUGNO 2012
echo FILIPPO LEONELLI `date`

#controllo numero parametri
if test $# -ne 2
	then echo NUMERO DI PARAMETRI PASSATI ERRATI
         exit 1
fi

#controllo sul tipo dei parametri
G=$1
K=$2

case $G in
	/*) ;;
     *) echo PARAMETRO 1 NOT OK
        exit 2
        ;;
esac

if test ! -d $G -a ! -x $G
then 
	echo PARAMETRO 1 NOT OK
	exit 2
fi

test $K -gt 0 >/dev/null 2>&1
case $? in
	0);;
     *) echo PARAMETRO 2 NOT OK
        exit 3
        ;;
esac

PATH=`pwd`:$PATH
export PATH
>/tmp/file$$

#chiamata ricorsiva
FCR.sh $G $K /tmp/file$$
echo CHIAMATA POST RICORSIONE :

for F in `cat /tmp/file$$ `
do 
    N=`wc -l <$F`
    C=`wc -c <$F`
    H=`expr $C / $N`

    echo FILE PASSATO ALLA PARTE C : $F $N $H

done

#rimozione file temporaneo
rm /tmp/file$$

echo HO FINITO TUTTO
