# OS_-请求页式存储管理-opt算法、lru算法、FIFO算法
参考文章：https://blog.csdn.net/qq_34777600/article/details/79508613

------------------------------------------------------------------------------
2020-01-04第一次提交
 
 操作系统（二）实验三
 实验要求
 是采用页式分配存储管理方案，并通过分析计算不同页面淘汰算法情况下的访问命中率来比较各种算法的优劣。另外也考虑到改变页面大小和实际存储器容量对计算结果的影响，从而可为算则好的算法、合适的页面尺寸和实存容量提供依据。
本程序是按下述原则生成指令序列的：
（1） 50%的指令是顺序执行的。
（2） 25%的指令均匀散布在前地址部分。
（3） 25%的指令均匀散布在后地址部分。
示例中选用最佳淘汰算法（OPT）、最近最少使用页面淘汰算法（LRU）、先进先出算法（fifo）计算页面命中率。

命中率 = 1 - 页面失效次数/页地址流长度

假定虚存容量为 32K，页面尺寸从 1K 至 8K，实存容量从 4 页至 32 页。

页面大小共四种，1K，2K，4K，8K.所以页面数量为32页，16页，8页和4页。
地址流共256个，初始化范围为[0-32767]
-----------------------------------------------------------------------------
