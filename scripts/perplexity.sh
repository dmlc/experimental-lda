#!/bin/bash

LOC=$1
# get list of files
while true
do
	COUNT=0
	for PHI in `ls $LOC/*.phi 2>/dev/null`
	do
		ITER=${PHI%.*}
		if [ -a "$ITER".llh ]
		then
			rm $PHI
			sleep 1
		else
			echo Processing "$ITER"
			dist/perplexity --dataset "$ITER" --testing-file "$2" --num-iterations 20 &
			COUNT=$((COUNT+1))
			sleep 1
		fi
		if [ $COUNT -gt 5 ]
		then
			wait
			COUNT=0
		fi
	done
	wait
	sleep 1
done

