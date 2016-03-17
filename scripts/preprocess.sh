#!/bin/bash

DATASET=$1
NUM_PACKS=`expr "$2" - 1`

MT=`wc -l "$DATASET".train | awk '{print $1}'`
M=`expr "$MT" / "$2"`
echo Spliting into files of "$M" documents
split -d --number=l/"$2" "$DATASET".train "$DATASET"-

if [ "$NUM_PACKS" -lt 10 ]
then
	for i in `seq 0 "$NUM_PACKS"`;
	do
    		echo "$DATASET"-0"$i"
    		sed -i '/^\s*$/d' "$DATASET"-0"$i"
		wc -l "$DATASET"-0"$i" | awk '{print $1}' > "$DATASET"-"$i".txt
    		cat "$DATASET"-0"$i" >> "$DATASET"-"$i".txt
    		rm "$DATASET"-0"$i"
	done
else
	for i in `seq 0 9`;
	do
    		echo "$DATASET"-0"$i"
    		sed -i '/^\s*$/d' "$DATASET"-0"$i"
    		wc -l "$DATASET"-0"$i" | awk '{print $1}' > "$DATASET"-"$i".txt
    		cat "$DATASET"-0"$i" >> "$DATASET"-"$i".txt
    		rm "$DATASET"-0"$i"
	done


	for i in `seq 10 $NUM_PACKS`;
	do
    		echo "$DATASET"-"$i"
    		sed -i '/^\s*$/d' "$DATASET"-"$i"
    		wc -l "$DATASET"-"$i" | awk '{print $1}' > "$DATASET"-"$i".txt
    		cat "$DATASET"-"$i" >> "$DATASET"-"$i".txt
    		rm "$DATASET"-"$i"
	done
fi

echo "$DATASET"-test
sed -i '/^\s*$/d' "$DATASET".test
wc -l "$DATASET".test | awk '{print $1}' > "$DATASET"-test.txt
cat "$DATASET".test >> "$DATASET"-test.txt

dist/datagen $DATASET $2
