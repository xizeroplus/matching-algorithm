## Introduction: 

In this repo, we implemented several state-of-the-art matching algorithms for content-based publish/subscribe systems. These matching algorithms include [Rein](https://ieeexplore.ieee.org/document/6848147/), [OpIndex](http://www.vldb.org/pvldb/vol7/p613-zhang.pdf), [H-Tree](https://ieeexplore.ieee.org/document/6663515/), [Tama](https://ieeexplore.ieee.org/document/5961731/), and [Siena](https://dl.acm.org/citation.cfm?doid=863955.863975). 

## Quick Start: 

```
g++ -std=c++11 chrono_time.h data_structure.h generator.h generator.cpp main.cpp printer.h printer.cpp rein.h rein.cpp util.h util.cpp -o rein
ulimit -s 819200
./rein
```

Please note that this project is based on C++11 standards, and requires a GNU C++ compiler no older than version 4.8.


## Input: 
The input parameters are read from paras.txt


## Output: 

Each line of the output contains four numbers, indicating the total number of subscriptions, the inserting time of each subscription, the matching time of each event, and the number of subscriptions that have been matched to at least one event. 

