### Graph topology experiment

For the graph topology experiment, we created four kinds of graphs, representing increasing levels of difficulty, based on the E.coli reference genome's 10000 first base pairs. 

The first graph, *the linear graph*, is <u>*a linear chain of nodes*</u>. aligning to the graph is equivalent to sequence-to-sequence alignment. The second graph, *the SNP graph* is a  *<u>linear chain of nodes with randomly inserted bubbles representing single nucleotide polymorphisms</u>*. The SNPs are distributed at <u>an average of one SNP per 10 base pairs</u>. The third graph, *the two-path graph*, is an artificial worst case graph for the bitvector algorithm. 

We simulated reads with 20x coverage(total 200 000 bp) from the reference using PBSIM, which produced 65 reads with an average length of 3kbp. In addition, we took a high coverage Illumina dataset, filtered the reads by using minimap2 to select reads which align to 





Linear-PBSIM-20000-19998

Linear-Illumina-20000-19998

SNP-