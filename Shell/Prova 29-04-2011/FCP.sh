#!/bin/sh
echo QUESTA E\' LA PROVA DEL 29 APRILE DEL 2011
echo FILIPPO LEONELLI `date`

#controllo sul numero dei parametri
if test $# -ne 3
then
	echo NUMERO PARAMETRI SBAGLIATO
	exit 1
fi

#controllo sul tipo dei parametri
case $1 in 
	/*) if test -d $1
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

case $2 in
	/*) echo PARAMETRO 2 NOT OK
        exit 7
        ;;
    */*)echo PARAMETRO 2 NOT OK
        exit 8
        ;;

     *) 
       echo PARAMETRO 2 OK

        ;;
esac


case $3 in
	[0-9])echo PARAMETRO 3 OK
          ;;
       *) echo PARAMETRO 3 NOT OK
        exit 5
        ;;
esac

#impostazione chiamata ricorsiva
PATH=`pwd`:$PATH
export PATH
NF=0 #variabile globale per contare i file
export NF
VAL=0
#chiamata ricorsiva
FCR.sh $1 $2 $NF $VAL

echo NUMERO DI FILE GLOBALMENTE TROVATI : $NF

if test $NF -lt $3
then 
	VAL=1
	FCR.sh $1 $2 $NF $VAL
fi

echo HO FINITO TUTTO
