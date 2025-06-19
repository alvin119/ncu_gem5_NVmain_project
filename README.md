# computer organization final project : gem5 + NVmain
## 修改的檔案
### Q2 enable L3 cache
- `gem5-525ce650e1a5bbe71c39d4b15598d6c003cc9f9e/configs/common/Caches.py`
- `gem5-525ce650e1a5bbe71c39d4b15598d6c003cc9f9e/configs/common/CacheConfig.py`
- `gem5-525ce650e1a5bbe71c39d4b15598d6c003cc9f9e/configs/common/Options.py`
- `gem5-525ce650e1a5bbe71c39d4b15598d6c003cc9f9e/src/cpu/BaseCPU.py`
- `gem5-525ce650e1a5bbe71c39d4b15598d6c003cc9f9e/src/mem/XBar.py`
### Q3 2 way and full way associativity
修改編譯指令
### Q4
- `gem5-525ce650e1a5bbe71c39d4b15598d6c003cc9f9e/configs/common/Caches.py`
- `gem5-525ce650e1a5bbe71c39d4b15598d6c003cc9f9e/src/mem/cache/replacement_policies/lfu_rp.cc`
- `gem5-525ce650e1a5bbe71c39d4b15598d6c003cc9f9e/src/mem/cache/replacement_policies/lfu_rp.hh`
### Q5 write back and write through policy
- `gem5-525ce650e1a5bbe71c39d4b15598d6c003cc9f9e/src/mem/cache/base.cc`
## 要求
- [ ] (Q1) GEM5 + NVMAIN BUILD-UP (40%)

- [ ] (Q2) Enable L3 last level cache in GEM5 + NVMAIN (15%) ( 看到 log 裡面有 L3 cache 的資訊 )

- [ ] (Q3) Config last level cache to 2-way and full-way associative cache and test performance (15%)
    - 必須跑benchmark quicksort在 2-way跟 full way (直接在 L3 cache implement，可以用 miss rate 判斷是否成功 )

- [ ] (Q4) Modify last level cache policy based on frequency based replacement policy (15%)

- [ ] (Q5) Test the performance of write back and write through policy based on 4-way associative cache with isscc_pcm(15%)
    - 必須跑 benchmark multiply 在 write through 跟 write back ( gem5 default 使用 write back，可以用 write request 的數量判斷 write through 是否成功 )

- [ ] Bonus (10%)
    - Design last level cache policy to reduce the energy consumption of pcm_based main memory
    - Baseline:LRU
## 相關指令、工具
### 和NVmain 混合編譯

- scons EXTRAS=../NVmain build/X86/gem5.opt
- scons -j4 EXTRAS=../NVmain build/X86/gem5.opt

### stats.txt觀察指標

- Q3(stats.txt):

    system.l3.overall_miss_rate::total

    system.l3.overall_hits::total

- Q4(stats.txt):

    system.l3.replacements

- Q5(terminal最底層output中):

    i0.defaultMemory.totalWriteRequests

### 編譯quicksort.c、multiply.c

- `gcc --static quicksort.c -o quicksort`
- `gcc --static multiply.c -o multiply`


### Q2測試enable l3cache:
```
./build/X86/gem5.opt configs/example/se.py -c tests/test-progs/hello/bin/x86/linux/hello --cpu-type=TimingSimpleCPU --caches --l2cache --l3cache --mem-type=NVMainMemory --nvmain-config=../NVmain/Config/PCM_ISSCC_2012_4GB.config
```

### Q3/Q4測試quicksort on 2 way and full way

- quicksort在../benchmark/:
```
./build/X86/gem5.opt configs/example/se.py -c ../benchmark/quicksort --cpu-type=TimingSimpleCPU --caches --l2cache --l3cache --l3_assoc=2 --l1i_size=32kB --l1d_size=32kB --l2_size=128kB --l3_size=1MB --mem-type=NVMainMemory --nvmain-config=../NVmain/Config/PCM_ISSCC_2012_4GB.config
```
- quicksort在./:
```
./build/X86/gem5.opt configs/example/se.py -c ./quicksort --cpu-type=TimingSimpleCPU --caches --l2cache --l3cache --l3_assoc=2 --l1i_size=32kB --l1d_size=32kB --l2_size=128kB --l3_size=1MB --mem-type=NVMainMemory --nvmain-config=../NVmain/Config/PCM_ISSCC_2012_4GB.config
```
    
### Q5 測試 write back and write through policy
```
./build/X86/gem5.opt configs/example/se.py -c ./multiply --cpu-type=TimingSimpleCPU --caches --l2cache --l3cache --l3_assoc=4 --l1i_size=32kB --l1d_size=32kB --l2_size=128kB --l3_size=1MB --mem-type=NVMainMemory --nvmain-config=../NVmain/Config/PCM_ISSCC_2012_4GB.config
```
## 各題目Result
### Q3 2-way and full-way
1. array size = 1000000(一百萬)
    - 2-way:

        system.l3.overall_miss_rate::total: 0.540001

    - full way(16384):

        system.l3.overall_miss_rate::total: 0.555192
2. array size = 500000(五十萬)
    - 2-way:

        system.l3.overall_miss_rate::total: 0.433384

    - full way(16384):

        system.l3.overall_miss_rate::total: 0.470628
3. array size = 2 * 10**5(二十萬)
    - 2-way:

        system.l3.overall_miss_rate::total: 0.182883

    - full way(16384):

        system.l3.overall_miss_rate::total: 0.182827

Q : 為何full way miss rate比2 way的還高?
A : gem5 cache預設使用LRU replacement policy，在arr-size為五十萬跟一百萬時大小分別為2MB、4MB，遠大於給定的cache size(l3 cache size = 1MB)，這會造成大量的conflict misses，此特殊情況可能導致2 way associativity比full way的miss rate還要低。
### Q4 frequency based replacement policy
- l3_assoc=2
1. array size = 1000000(一百萬)
    - original policy:

        system.l3.replacements 263741

    - LFU policy: 

        system.l3.replacements 346767
2. array size = 500000(五十萬)
    - original policy:

        system.l3.replacements 91438

    - LFU policy: 

        system.l3.replacements 113143
### Q5 write back and write through policy

- write back(gem5 default) :
    
    數據首先寫入快取，並且只有在快取中的數據被替換時，才會寫入主記憶體。
    
    i0.defaultMemory.totalWriteRequests 692
    
- write through:
    
    數據被寫入快取時，它同時也會立即寫入主記憶體。
    
    i0.defaultMemory.totalWriteRequests 13906218


## 常見問題

- 記憶體空間不足
- Ubuntu 版本不對，有問題的話建議用 18.04
- 執行 benchmark 的時候，因為 gem5 本身速度緩慢，會誤以為程式卡住，但其實他仍然有在執行
- 跑完程式後，可以從 m5out 的資料夾去查看 stat.txt
- 更改完 gem5 的檔案記得要編譯再重新執行
- gem5 有 debug flag 的功能
build/X86/gem5.opt --debug-flags=Exec configs/learning_gem5/part1/simple.py | head -n 50
