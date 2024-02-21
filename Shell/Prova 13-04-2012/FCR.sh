#!/bin/sh

cd $1
nliv= #variabile per memorizzare a che livello mi trovo ora

if test $2 = A
then
	#faccio la fase A
	nliv=`cat $3`
	nliv=`expr $nliv + 1`
	echo $nliv > $3
#faccio la chiamata ricorsiva
	for i in *
	do
		if test -d $i -a -x $i
		then
			FCR.sh `pwd`/$i $2 $3
		fi
	done

else
	#faccio la fase B
	nliv=`cat $3`
	nliv=`expr $nliv + 1`
	echo $nliv > $3
	if test $nliv -eq $4
	then
		echo LE INFORMAZIONI DI TUTTI I FILE E DIRECTORY PRESENTI A QUESTO LIVELLO SONO :
		for i in *
		do
			ls -l $i
		done
	fi

	#faccio la chiamata ricorsiva
	for i in *
	do
		if test -d $i -a -x $i
		then
			FCR.sh `pwd`/$i $2 $3 $4
		fi
	done

fi
