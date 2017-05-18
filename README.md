**编译指令:**
g++ -std=c++11 chrono_time.h data_structure.h generator.h generator.cpp main.cpp printer.h printer.cpp rein.h rein.cpp util.h util.cpp -o rein

（c++11需要g++ 4.8以上的版本支持）

**输入参数:**
subNum和bucketNum作为命令行参数传入，其余参数从文件paras.txt中读入。

**运行:**
ulimit -s 819200

./rein (subNum) (bucketNum)

**输出:**
每行六个数值，依次表示1.桶总数 2.单个事件用时 3.单个事件循环执行总次数 4.alpha 5.beta 6.gamma。

45	2.526677	199777.569500	0.000021	0.000010	0.000120

**脚本文件:**
bash test.sh
