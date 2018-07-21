
# KMC

The project is a fork of [KMC](https://github.com/refresh-bio/KMC), see its doc.

This is a modified version of KMC, in particular there are a `kmc_dump` version modified for generate the kmers occurances as input of an Hadoop Project.


## kmc_dump_inverted 

`kmc_dump_inverted` prints in inverted way the result of `kmc_dump`:

```txt
...
21876582	ACT
...
```

## kmc_dump_indexed

`kmc_dump_indexed` prints with the line also the sequence identifier, based on the database name previusly generated from the program kmc.
The result is as follows:

```txt
...
seq7.fasta	ACT	21876582
...
```
