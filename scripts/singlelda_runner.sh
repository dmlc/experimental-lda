#!/bin/bash

DATASETS="enron" 
METHODS="lightLDA"
NUM_ITER="100"
NUM_TOPICS="100"

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
	echo Using single thread total on Amazon EC2 c4.8xlarge | tee -a $DIR_NAME/log.txt

	#run
	#valgrind --leak-check=yes
	dist/singleLDA --method "$METHOD" --testing-mode net --alpha 50 --beta 0.1 --num-topics $NUM_TOPICS --num-iterations $NUM_ITER --output-state-interval 10 --output-model $DIR_NAME --num-top-words 15 --dataset data/"$DATASET" | tee -a $DIR_NAME/log.txt

done
done
echo 'done'
