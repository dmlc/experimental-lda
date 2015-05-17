Program for converting (docID wordID count) format to Mallet format of 1 doc per line.

Command line:
$ ./fp <document_file> <dictionary_file> <output_file> <number_of_test_docs> [shuffle]

- <document_file>: contains (docID wordID count) triplet with first three lines being: 1. Number of docs, 2. Vocab size, 3. Number of entries
- <dictionary_file>: contains list of words in order of wordID
- <output_file>: The name of output file. It will produce "output_file.train" and "output_file.test"
- <number_of_test_docs>: The number of documents to be kept held out as a test set
- <shuffle>: 0/1 to denote if order of words need to be shuffled in each documents

Example:
An example script "runner.sh" has been provided which downloads datasets from UCI repository and converts it into Mallet format.
(nips, enron, nytimes, pubmed)