#!/bin/bash

DATASETS="nytimes" 
METHODS="unigram"

for DATASET in $DATASETS
do
for METHOD in $METHODS
do
	#Create the directory structure
	time_stamp=`date "+%b_%d_%Y_%H.%M.%S"`
	DIR_NAME='out'/$DATASET/$METHOD/$time_stamp/
	mkdir -p $DIR_NAME

	#save details about experiments in an about file
	echo Running ngram inference using $METHOD | tee -a $DIR_NAME/log.txt
	echo For dataset $DATASET | tee -a $DIR_NAME/log.txt
	echo with results being stored in $DIR_NAME

	#run
	#valgrind --leak-check=full --show-leak-kinds=all 
	dist/unigram --method "$METHOD" --testing-mode net --beta 0.1 --output-model $DIR_NAME --num-top-words 15 --dataset data/"$DATASET" | tee -a $DIR_NAME/log.txt

done
done
echo 'done'
