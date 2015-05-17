Program for single threaded LDA implementation using:

1. Plain vanilla Gibbs sampling
2. Sparse LDA of Yao09
3. Alias LDA
4. F+LDA
5. Vitter LDA
6. light LDA of Yuan14

The program runs in three mode:
1. No likelihood evaluation - useful for just obtaining timing in a hurry
2. Likelihood evaluation on training data
3. Likelihood evaluation on testing data

Also there is preprocessing definition #COMP_LLH, which if defined to be 1, computes likelihood after every iteration.

Command line:
$ ./method -xxx [-alpha <real+>] [-beta <real+>] [-ntopics <int>] [-niters <int>] [-odir <string>] [-savestep <int>] [-twords <int>] -dfile <string> [-tfile <string>]
in which (parameters in [] are optional):

-method: can be {simpleLDA, sparseLDA, aliasLDA, FTreeLDA, lightLDA} corresponding to the six methods described above.
-xxx: can be {-nt, -set, -net} corresponding to the three modes described above.
-alpha <real+>: The value of alpha, hyper-parameter of LDA. The default value of alpha is 50 / ntopics.
-beta <real+>: The value of beta, hyper-parameter of LDA. The default value of beta is 0.1.
-ntopics <int>: The number of topics. The default value is 100.
-niters <int>: The number of Gibbs sampling iterations. The default value is 2000.
-odir <string>: The location of
-savestep <int>: The step (counted by the number of Gibbs sampling iterations) at which the LDA model is saved to hard disk. The default value is 200.
-twords <int>: The number of most likely words for each topic. The default value is zero. 
-dfile <string>: The input training data file.

Output files:
1. *.time: The time taken for each iteration
2. *.twords: The top words in each topic
3. *.tassign: The topic assignment of each word in the corpus
4. *.llh: The likelihood after each iteration
5. *.phi: The p(word|topic) matrix

To compile the program:
On Linux:
1. Use Intel compiler for much better performance: simply hit make
2. If you do not have Intel compiler: hit make gcc
On Windows:
1. Use Intel compiler for much better performance: simply hit make intelwin
2. If you do not have Intel compiler and use Visual C++: hit make windows

Also we have provided an example script "runner.sh" to run the LDA on UCI datasets located at ../data and processed according to readme file therein.