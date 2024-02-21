#!/bin/sh

cd $1
nl=0 #variabile che mi serve per memorizzare il numero di linee del file corrente
nf=0 #variabile per memorizzare il numero di file che rispettano i vincoli
for F in *
do
	if test -f $F -a -r $F
	then
		case $F in
			$2?$2?$2) nl=`wc -l <$F`
                      if test $nl -eq $3
                      	then nf=`expr $nf + 1`
                             echo `pwd`/$F >> $4
                      fi
                      ;;
                  *);;
        esac
    fi
done

if test $nf -ge 1
then
	echo DIRECTORY CHE SODDISFA I VINCOLI : `pwd`
fi

for i in *
do
	if test -d $i -a -x $i
	then
		FCR.sh `pwd`/$i $2 $3 $4
	fi
done
