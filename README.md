# Single Machine implementation of LDA

## Modules
1. `parallelLDA` contains various implementation of multi threaded LDA
2. `singleLDA` contains various implementation of single threaded LDA
3. `topwords` a tool to explore topics learnt by the LDA/HDP
4. `perplexity` a tool to calculate perplexity on another dataset using word|topic matrix
5. `datagen` packages txt files for our program
6. `preprocessing` for converting from UCI or cLDA to simple txt file having one document per line


## Organisation
1. All codes are under `src` within respective folder
2. For running Topic Models many template scripts are provided under `scripts`
3. `data` is a placeholder folder where to put the data
4. `build` and `dist` folder will be created to hold the executables
 
## How to use
We will show how to run our LDA on an [UCI bag of words dataset](https://archive.ics.uci.edu/ml/datasets/Bag+of+Words)

1. First of all compile by hitting make

   ```bash
     make
   ```

   if you have Intel Compiler, then you can instead

   ```bash
     make intel
   ```

   Also you can selectively compile individual modules by specifying

   ```bash
     make <module-name>
   ```

   or clean individually by

   ```bash
     make clean-<module-name>
   ```

2. Download example dataset from UCI repository. For this a script has been provided.

   ```bash
     scripts/get_data.sh
   ```

3. Prepare the data for our program

   ```bash
     scripts/prepare.sh data/nytimes 0
   ```

   For other datasets replace nytimes with dataset name or location.

4. Run LDA!

   ```bash
     scripts/lda_runner.sh
   ```

   Inside the `lda_runner.sh` all the parameters e.g. number of topics, hyperparameters of the LDA, number of threads etc. can be specified. By default the outputs are stored under `out/`. Also you can specify which inference algorithm of LDA you want to run:
   1. `simpleLDA`: Plain vanilla Gibbs sampling
   2. `sparseLDA`: Sparse LDA of Yao09
   3. `aliasLDA`: Alias LDA
   4. `FTreeLDA`: F++LDA
   5. `lightLDA`: light LDA of Yuan14
