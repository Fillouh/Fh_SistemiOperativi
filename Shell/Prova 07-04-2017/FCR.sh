#!/bin/sh

cd $1

for i in *
do
	case $i in
		$2) if test -f $i -a -r $i
            then
            	sort -f $i > $i-sorted
            	echo `pwd`/$i-sorted >> $3
            fi
            ;;
        *);;
    esac
done

for i in *
do
	if test -d $i -a -x $i
	then
		FCR.sh `pwd`/$i $2 $3
	fi
done
