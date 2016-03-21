#!/bin/bash

#Download NIPS from UCI repository
wget -P data/ https://archive.ics.uci.edu/ml/machine-learning-databases/bag-of-words/docword.nips.txt.gz
gunzip data/docword.nips.txt.gz
wget -P data/ https://archive.ics.uci.edu/ml/machine-learning-databases/bag-of-words/vocab.nips.txt
dist/preprocessing UCI data/docword.nips.txt data/vocab.nips.txt data/nips 500 1
rm -rf data/docword.nips.txt data/vocab.nips.txt

#Download ENRON from UCI repository
wget -P data/ https://archive.ics.uci.edu/ml/machine-learning-databases/bag-of-words/docword.enron.txt.gz
gunzip data/docword.enron.txt.gz
wget -P data/ https://archive.ics.uci.edu/ml/machine-learning-databases/bag-of-words/vocab.enron.txt
dist/preprocessing UCI data/docword.enron.txt data/vocab.enron.txt data/enron 500 1
rm -rf data/docword.enron.txt.gz data/docword.enron.txt data/vocab.enron.txt

#Download NYTIMES from UCI repository
wget -P data/ https://archive.ics.uci.edu/ml/machine-learning-databases/bag-of-words/docword.nytimes.txt.gz
gunzip data/docword.nytimes.txt.gz
wget -P data/ https://archive.ics.uci.edu/ml/machine-learning-databases/bag-of-words/vocab.nytimes.txt
dist/preprocessing UCI data/docword.nytimes.txt data/vocab.nytimes.txt data/nytimes 1500 1
rm -rf data/docword.nytimes.txt.gz data/docword.nytimes.txt data/vocab.nytimes.txt
	
#Download PUBMED from UCI repository
wget -P data/ https://archive.ics.uci.edu/ml/machine-learning-databases/bag-of-words/docword.pubmed.txt.gz
gunzip data/docword.pubmed.txt.gz
wget -P data/ https://archive.ics.uci.edu/ml/machine-learning-databases/bag-of-words/vocab.pubmed.txt
dist/preprocessing UCI data/docword.pubmed.txt data/vocab.pubmed.txt data/pubmed 10000 1
rm -rf data/docword.pubmed.txt.gz data/docword.pubmed.txt data/vocab.pubmed.txt

echo 'done'
