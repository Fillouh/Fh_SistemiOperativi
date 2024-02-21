#!/bin/sh

cd $1

for i in *
do
	if test -f $i -a -r $i -a -w $i
	then
		case $i in
			$2\.txt) c=`wc -c <$i`
                     nome=`pwd`/$i
                     riga="$c $nome"
                     echo $riga
                     echo $riga >> $3
                     ;;
                *);;
        esac
    fi
done

for i in *
do
	if test -d $i -a -x $i
	then
		FCR.sh `pwd`/$i $2 $3
	fi
done
