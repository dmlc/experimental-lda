#!/bin/bash

DATASETS="pubmed" 
METHODS="lightLDA"
NUM_ITER="1000"
NUM_TOPICS="1000"
NST="20"
NTT="4"

for DATASET in $DATASETS
do
for METHOD in $METHODS
do
	#Create the directory structure
	time_stamp=`date "+%b_%d_%Y_%H.%M.%S"`
	DIR_NAME='out'/$DATASET/$METHOD/$time_stamp/
	mkdir -p $DIR_NAME

	#save details about experiments in an about file
	echo Running LDA inference using $METHOD | tee -a $DIR_NAME/log.txt
	echo For dataset $DATASET | tee -a $DIR_NAME/log.txt
	echo For number of iterations $NUM_ITER | tee -a $DIR_NAME/log.txt
	echo For number of topics $NUM_TOPICS | tee -a $DIR_NAME/log.txt
	echo with results being stored in $DIR_NAME
	echo Using $NST sampler threads and $NTT updater threads on bros | tee -a $DIR_NAME/log.txt

	#run
	#valgrind --leak-check=yes
	dist/sillyLDA --method "$METHOD" --testing-mode net --num-sampling-threads $NST --num-table-threads $NTT --num-topics $NUM_TOPICS --num-iterations $NUM_ITER --output-state-interval 1 --output-model $DIR_NAME --num-top-words 15 --dataset data/"$DATASET" | tee -a $DIR_NAME/log.txt

done
done
echo 'done'
