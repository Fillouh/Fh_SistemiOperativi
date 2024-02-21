#!/bin/sh

cd $1
nf=0
for i in *
do
	if test -f $i -a -r $i
	then
		case $i in
			*\.$2) nf=`expr $nf + 1`
                   ;;
                *);;
        esac
    fi
done

if test $nf -ge 1
then
	echo `pwd` >> $3
fi

for i in *
do
	if test -d $i -a -x $i
	then
		FCR.sh `pwd`/$i $2 $3
	fi
done
