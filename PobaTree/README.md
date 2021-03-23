## PoBa-Tree build

1. enter the directory of this project: ```cd PobaTree```
2. use cmake to build this project: ```mkdir build; cd build; cmake ..```
3. compile: ```make```

## useage

after compile, there will be two executable files: PobaTree and dataFileGeneratro(a helper program to generator data, can be ignored). The `PobaTree/params.txt` file is the configure file to configure params. Parameters are:
```
    int subs;     // Number of subscriptions.
    int pubs;     // Number of publications.
    int atts;     // Total number of attributes, i.e. dimensions.
    int cons;     // Number of constraints(predicates) in one sub.
    int m;        // Number of constraints in one pub.
    int attDis;   // The distribution of attributes in subs and pubs. 0:uniform distribution | 1:Zipf distribution
    int valDis;   // The distribution of values in subs and pubs. 0:uniform, 1: both zipf, -1: sub zipf but pub uniform
    int valDom;   // Cardinality of values.
    double alpha; // Parameter for Zipf distribution.
    double width; // Width of a predicate.
```

copy `PobaTree/params.txt` file to the build directory: `cp ../params.txt .`

run pobaTree: `./PobaTree`

## code
main class is `PobaTree` in `PobaTree.h`, there are two match calls: 

1. `PobaTree::match` is used for single thread match
2. `pobaTree::match_parallel` is used for multi-thread match