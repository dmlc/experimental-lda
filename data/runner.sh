#!/bin/bash

g++ -O3 -std=c++11 preprocessing.cpp -o "fp"

#Download NIPS from UCI repository
wget https://archive.ics.uci.edu/ml/machine-learning-databases/bag-of-words/docword.nips.txt.gz
gunzip docword.nips.txt.gz
wget https://archive.ics.uci.edu/ml/machine-learning-databases/bag-of-words/vocab.nips.txt
./fp docword.nips.txt vocab.nips.txt nips 500 0
rm -rf docword.nips.txt vocab.nips.txt

#Download ENRON from UCI repository
wget https://archive.ics.uci.edu/ml/machine-learning-databases/bag-of-words/docword.enron.txt.gz
gunzip docword.enron.txt.gz
wget https://archive.ics.uci.edu/ml/machine-learning-databases/bag-of-words/vocab.enron.txt
./fp docword.enron.txt vocab.enron.txt enron 500 0
rm -rf docword.enron.txt.gz docword.enron.txt vocab.enron.txt

#Download NYTIMES from UCI repository
wget https://archive.ics.uci.edu/ml/machine-learning-databases/bag-of-words/docword.nytimes.txt.gz
gunzip docword.nytimes.txt.gz
wget https://archive.ics.uci.edu/ml/machine-learning-databases/bag-of-words/vocab.nytimes.txt
./fp docword.nytimes.txt vocab.nytimes.txt nytimes 500 0
rm -rf docword.nytimes.txt.gz docword.nytimes.txt vocab.nytimes.txt
	
#Download PUBMED from UCI repository
wget https://archive.ics.uci.edu/ml/machine-learning-databases/bag-of-words/docword.pubmed.txt.gz
gunzip docword.pubmed.txt.gz
wget https://archive.ics.uci.edu/ml/machine-learning-databases/bag-of-words/vocab.pubmed.txt
./fp docword.pubmed.txt vocab.pubmed.txt pubmed 10000 0
rm -rf docword.pubmed.txt.gz docword.pubmed.txt vocab.pubmed.txt
	
wget http://qwone.com/~jason/20Newsgroups/20news-bydate-matlab.tgz
wget http://qwone.com/~jason/20Newsgroups/vocabulary.txt
tar -xzf 20news-bydate-matlab.tgz
`echo 11269 > docword.20news.txt`
`echo 61188 >> docword.20news.txt`
`echo 1467345 >> docword.20news.txt`
cat 20news-bydate/matlab/train.data >> docword.20news.txt
./fp docword.20news.txt vocabulary.txt 20news1.txt 0 0
`echo 7505 > docword.20news.txt`
`echo 61188 >> docword.20news.txt`
`echo 967874 >> docword.20news.txt`
cat 20news-bydate/matlab/test.data >> docword.20news.txt
./fp docword.20news.txt vocabulary.txt 20news2.txt 0 0
`tail -f 11269 20news1.txt > 20news.txt`
`tail -f 7505 20news2.txt >> 20news.txt`
`echo 500 > 20news.test`
`tail -f 500 20news.tzt > 20news.test`
`tail -f 500 20news.tzt > 20news.test`
`head -f 18274 20news.tzt > 20news.train`
rm -rf 20news-bydate docword.20news.txt fp
echo 'done'
