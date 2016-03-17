#!/bin/bash

DATASETS="wiki" 
METHODS="cvb0LDA"
NUM_ITER="100"
NUM_TOPICS="1000"
NST="50"

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
	echo For dataset $dataset | tee -a $DIR_NAME/log.txt
	echo For number of iterations $NUM_ITER | tee -a $DIR_NAME/log.txt
	echo For number of topics $NUM_TOPICS | tee -a $DIR_NAME/log.txt
	echo with results being stored in $DIR_NAME
	echo Using '$NST' sampling thread per vCPU on bros | tee -a $DIR_NAME/log.txt

	#run
	mpirun -n 8 -binding "domain=socket" -machinefile ~/hosts -print-rank-map dist/cvb0LDA --method "$METHOD" --testing-mode nt --num-sampling-threads $NST --num-topics $NUM_TOPICS --num-iterations $NUM_ITER --output-state-interval 1 --output-model $DIR_NAME --num-top-words 15 --dataset data/"$DATASET" | tee -a $DIR_NAME/log.txt
	#mpirun -v -cpus-per-proc 9 -np 8 --report-bindings -machinefile config/hosts dist/cvb0LDA --method "$METHOD" --testing-mode nt --num-sampling-threads $NST --num-topics $NUM_TOPICS --num-iterations $NUM_ITER --output-state-interval 1 --output-model $DIR_NAME --num-top-words 15 --dataset data/"$DATASET" | tee -a $DIR_NAME/log.txt

	#while !([ -s "$DIR_NAME"/"$DATASET"-"$NUM_ITER".llh ])
        #do
        #        scripts/perplexity.sh "$DIR_NAME"/"$DATASET" data/"$DATASET"-test.dat
        #done
        #rm "$DIR_NAME"/*phi
done
done
echo 'done'
