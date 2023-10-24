# NNSearch: A Unified NN Search Framework

Due to restrictions imposed by the company's agreement, we are unable to release the internal code for building indexes and conducting the NN search. However, we now release a unified NN search framework based on the [hnswlib](https://github.com/nmslib/hnswlib). While it may not be fully optimized, one advantage is that it allows for a fairer comparison of search performance with hnswlib, even if the latter employs a hierarchical indexing structure.

## Features
* support various indexing graphs (TSDG$^+$, DPG, NSG, SSG, Vamana and HNSW), while only one universal api.
    * Automatically select the load function based on the suffix of the index graph.
    * `.ivecs` for k-NN graph, shortcut graph, TSDG$^+$, and [DPG](https://github.com/DBAIWangGroup/nns_benchmark)
    * `.nsg` for [NSG](https://github.com/ZJULearning/nsg) 
    * `.ssg` for [SSG](https://github.com/ZJULearning/SSG) 
    * `.hnsw` for [HNSW](https://github.com/nmslib/hnswlib)
    * [Vamana](https://github.com/microsoft/DiskANN)'s indexing graph has no suffix. 
* Fast NN search performance and has a fair compaision with hnswlib. 
    * no change to hnsw indexing and NN search. 
    * our NN search = hnsw layer0's NN search + random seeds. 

## Compile 

* Please make sure `Boost` is installed. On ubuntu, it can be installed by `sudo apt install libboost-all-dev`.

```shell
cd hnswlib
mkdir build && cd build 
cmake .. && make -j
cd ../..
```

## Pre-built Indexing Graphs are available

* [Google Drive](https://drive.google.com/file/d/1Se5q3aYV5H26u21-uIBh9Lq-qgpSN3mQ/view?usp=sharing)
* Only SIFT1M and DEEP1M have indexing graphs for all comparison methods, while other datasets only have index graphs for TSDG$^+$.

## Usage 
1. run NN search performance curve
this will load indexing graph to run NN-Search and save records.
warning: NN search works on single-thread. It may take several minutes.

```shell
./hnswlib/build/uni_nnsearch \
--base_path=/home/jfwang/data/sift1m/sift1m_base.fvecs \
--query_path=/home/jfwang/data/sift1m/sift1m_query.fvecs \
--gt_path=/home/jfwang/data/sift1m/sift1m_gt.ivecs \
--index_path=/home/jfwang/data/sift1m/sift1m_TSDGed_K200_SavMem_XNDesc_SCG_1.2_4_50.ivecs \
>> ./nns_records/sift1m_TSDG_plus.record
```

2. visualization
after all records in `./nns_records` folder. we can call `draw_fig.py` to show curves.

```shell
python draw_fig.py
```


## Note
1. Only `float` datatype is support as hnswlib does. 
2. Only search performance curve in out paper's Fig 6 - 8 can be made. We have some un-released version of code, which optimizes the memory cost during NN search. 
3. For full API, plz run `./hnswlib/build/uni_nnsearch -h` 
4. Our API source codes are listed at `hnswlib/examples/cpp/uni_nnsearch.cpp` and `hnswlib/hnswlib/dataloader.h`. Have fun with it. 
