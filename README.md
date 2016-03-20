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


## Requirements
1. gcc >= 5.0 or Intel Compiler 2016 for using C++14 features
2. split >= 8.21 (part of GNU coreutils)

## How to use
We will show how to run our LDA on an [UCI bag of words dataset](https://archive.ics.uci.edu/ml/datasets/Bag+of+Words)

1. First of all compile by hitting make

   ```bash
     make
   ```

2. Download example dataset from UCI repository. For this a script has been provided.

   ```bash
     scripts/get_data.sh
   ```

3. Prepare the data for our program

   ```bash
     scripts/prepare.sh data/nytimes 1
   ```

   For other datasets replace nytimes with dataset name or location.

4. Run LDA!

   ```bash
     scripts/lda_runner.sh
   ```

   Inside the `lda_runner.sh` all the parameters e.g. number of topics, hyperparameters of the LDA, number of threads etc. can be specified. By default the outputs are stored under `out/`. Also you can specify which inference algorithm of LDA you want to run:
   1. `simpleLDA`: Plain vanilla Gibbs sampling by [Griffiths04](http://www.pnas.org/content/101/suppl_1/5228.abstract)
   2. `sparseLDA`: Sparse LDA of [Yao09](http://dl.acm.org/citation.cfm?id=1557121)
   3. `aliasLDA`: Alias LDA
   4. `FTreeLDA`: F++LDA (inspired from [Yu14](http://arxiv.org/abs/1412.4986)
   5. `lightLDA`: light LDA of [Yuan14](http://arxiv.org/abs/1412.1576)

The make file has some useful features:

- if you have Intel Compiler, then you can instead

   ```bash
     make intel
   ```

- or if you want Intel's cross-file optimization (ipo), then hit
   
   ```bash
     make inteltogether
   ```

- Also you can selectively compile individual modules by specifying

   ```bash
     make <module-name>
   ```

- or clean individually by

   ```bash
     make clean-<module-name>
   ```

## Performance
Based on our evaluation F++LDA works the best in terms of both speed and perplexity on a held-out dataset. For example on Amazon EC2 c4.8xlarge, we obtained more than 25 million/tokens per second. Below we provide performance comparison against various inference procedures on publicaly available datasets.

### Datasets

| Command    | Description                                    |
| ---        | ---                                            |
| git status | List all new or modified files                 |
| git diff   | Show file differences that haven't been staged |


|  Dataset     |  V        |  L              |  D           |  L/V      |  L/D      |
| ------------ | --------- | --------------- | ------------ | --------- | --------- |
|  20 News     |  18,127   |  1,191,840	   |	11,266	   |	65.75    |  105.79   |
|  HEP         |  37,729	|	1,548,935	   |	27,770	   |	41.05	   |	55.78 	|
|  Reuters     |	69,973   |	2,624,373      |	14,377	   |	37.51	   |	182.54 	|
|  Enron       |	28,102	|	6,412,174      |	40,861	   |	228.18   |	160.86   |
|  ACM         |	133,325  |  12,258,310     |	132,032	   |	41.83    |  93.06    |
|  NY Times    |  101,330  |  99,542,127     |  299,753     |  982.36   |  332.08   |
|  PubMed      |  141,043  |  737,869,085    |  8,200,000   |  5,231.52	|	89.98    |
|  Wikipedia   |	210,218  |  1,614,349,889  |  3,731,325   |  7,679.41	|	432.65 	|

   Experimental datasets and their statistics. `V` denotes vocabulary size, `L` denotes the number of training tokens, `D` denotes
   the number of documents, `L/V` indicates the average number of occurrences of a word, `L/D` indicates the average length of a
   document.
  
### log-Perplexity with time

<img src=https://raw.githubusercontent.com/dmlc/experimental-lda/master/nytimes_llh_v_time.jpg width=400/>
