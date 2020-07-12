# ねらい
1. 脳を模倣したアルゴリズムの実行
1. 2025年までに1PFlopsを達成
1. １台のPCを１PCノードとして、それらを複数台接続して分散計算できる基盤を確立  

## 2025年に想定するPC
20205年は2020年を基準として計算機の性能が10倍に向上している想定。  
GPU：100 TFlops ×4  
CPU：10 TFlops
※GPUは認識アルゴリズムを実行し、CPUは学習アルゴリズムの実行を想定。  
概ね、400 TFlopsで実行効率が20%として、80 TFlops程度の計算量を想定。

## 第一段階
OpenCLを利用して１PCノードで実行計算効率20%以上で動作するソフトウェア基盤を作成する。

