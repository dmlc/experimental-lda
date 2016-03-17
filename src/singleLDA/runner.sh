#!/bin/bash

DATASETS="20news acm enron gpol hep"
METHODS="aliasLDA FTreeLDA sparseLDA lightLDA"
NUM_ITER="1000"
NUM_TOPICS="64"

for DATASET in $DATASETS
do
for METHOD in $METHODS
do
	#Create the directory structure
	time_stamp=`date "+%b_%d_%Y_%H.%M.%S"`
	DIR_NAME='res'/$DATASET/$METHOD/$time_stamp/
	mkdir -p $DIR_NAME

	#save details about experiments in an about file
	`echo 'Running LDA inference using' $METHOD | tee -a $DIR_NAME/log.txt`
	`echo 'For dataset' $dataset | tee -a $DIR_NAME/log.txt`
	`echo 'For number of iterations' $NUM_ITER | tee -a $DIR_NAME/log.txt`
	`echo 'For number of topics' $NUM_TOPICS | tee -a $DIR_NAME/log.txt`
	echo 'with results being stored in' $DIR_NAME
	`echo 'Using single sampling thread' | tee -a $DIR_NAME/log.txt`

	#run
	./LDA --method "$METHOD" --testing-mode net --num-topics $NUM_TOPICS --num-iterations $NUM_ITER --output-model $DIR_NAME --num-top-words 15 --training-file ../data/"$DATASET".train --testing-file "$DATASET".test | tee -a $DIR_NAME/log.txt
done
done

echo 'done'
