#!/bin/bash

DATASETS="acm" 
METHODS="compactHDP"
NUM_ITER="1000"
NUM_TOPICS="1000"

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
	dist/sparseHDP --method "$METHOD" --testing-mode net --alpha 10 --beta 0.1 --gamma 10 --num-topics $NUM_TOPICS --num-iterations $NUM_ITER --output-state-interval 10 --output-model $DIR_NAME --num-top-words 15 --dataset data/"$DATASET" | tee -a $DIR_NAME/log.txt

done
done
echo 'done'
