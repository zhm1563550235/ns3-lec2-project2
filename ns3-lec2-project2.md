# Ns3仿真报告————拓扑结构
##代码分析
   网络拓扑结构
   Number of wifi or csma nodes can be increased up to 250|
                               Rank 0 |  Rank 1
  ----------------------------|----------------------------
  Wifi 10.1.3.0
                  AP
    *   *    *    *
    |   |    |    |  10.1.1.0
   n5   n6   n7   n0 ------------------n1   n2   n3   n4
                      point-to-point   |     |    |    | 
                                       *     *    *    *
                                       AP
                                       wifi 10.1.2.0
在p2p网络左边，我们添加了10.1.3.0的WiFi网络。其中n0为AP节点，n5、6、7为STA节点。通过设置nWifi来控制仿真中出现的STA节点数。
