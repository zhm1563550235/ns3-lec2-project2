# Ns3仿真报告————拓扑结构
##代码分析
  > 网络拓扑结构
  > Number of wifi or csma nodes can be increased up to 250|
  >                    Rank 0 |  Rank 1
  >----------------------------|----------------------------
  >Wifi 10.1.3.0
  >               AP
  >  *   *    *    *
  >  |   |    |    |  10.1.1.0
  > n5   n6   n7   n0 ------------------n1   n2   n3   n4
  >                    point-to-point   |     |    |    | 
  >                                     *     *    *    *
  >                                     AP
  >                                     wifi 10.1.2.0
  
 在p2p网络左边，我们添加了10.1.3.0的WiFi网络。其中n0为AP节点，n5、6、7为STA节点。通过设置nWifi来控制仿真中出现的STA节点数。

```
PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer p2pDevices;
  p2pDevices = pointToPoint.Install (p2pNodes);
  ```
  以上代码实例化PointToPointHelper并且设置相关默认属性，在生成的设备上创建5Mbps的发射器和2ms的延迟信道，然后在节点和信道间安装设备。

```
YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();
  YansWifiPhyHelper phy = YansWifiPhyHelper::Default ();
  phy.SetChannel (channel.Create ());

  WifiHelper wifi = WifiHelper::Default ();
  wifi.SetRemoteStationManager ("ns3::AarfWifiManager");

  NqosWifiMacHelper mac = NqosWifiMacHelper::Default ();

  Ssid ssid = Ssid ("wifi_1");
  mac.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssid),
               "ActiveProbing", BooleanValue (false));

  NetDeviceContainer sta1Devices;
  sta1Devices = wifi.Install (phy, mac, wifi1StaNodes);

  mac.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid));

  NetDeviceContainer ap1Devices;
  ap1Devices = wifi.Install (phy, mac, wifi1ApNode);
  ```
  
  该代码段构建了无线设备与无线节点之间的互连通道。首先配置PHY和通道助手，使用默认的PHY层配置和信道模型。PHY配置后，使用 NqosWifiMacHelper对象设置MAC参数。站的具体参数配置完成后，在MAC层和PHY层可以调用安装方法来创建站的无线设备。即代码 ``
  NetDeviceContainer sta1Devices;sta1Devices = wifi.Install (phy, mac, wifi1StaNodes);`` 给STA节点配置无线之后，还需要配置AP（接入点）节点。
  
  ```
  //分配IP地址
  Ipv4AddressHelper address;

  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces;
  p2pInterfaces = address.Assign (p2pDevices);
//P2P信道
  address.SetBase ("10.1.2.0", "255.255.255.0");
  address.Assign (sta1Devices);
  address.Assign (ap1Devices);

  address.SetBase ("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer wifi2StaInterfaces;
  wifi2StaInterfaces = address.Assign (sta2Devices);
  address.Assign (ap2Devices);
  ```
  以上代码使用Ipv4AddressHelper分配IP地址给设备接口。首先使用网络10.1.1.0创建2个点对点设备需要的2个地址。然后使用网络10.1.2.0分配地址给STA设备1和无线网络的AP1。最后网络10.1.3.0分配地址给STA设备2和无线网络的AP2。
  
  运行整个程序，得到结果如图所示：
  ![编译结果](http://7xrn7f.com1.z0.glb.clouddn.com/16-6-20/47230805.jpg)
  从编译结果可以看到，UDP回显客户端发送1024byte的数据分组给地址为10.1.3.1的服务器，此时客户端在地址为10.1.2.1的无线网络处。当UDP回显服务器收到回显数据分组后，同样会发送回相同的字节给回显客户端。并且在顶层目录，生成了4个跟踪文件。“third-0-0.pcap”对应节点0上的点到点设备，“third-1-0.pcap”对应节点1上的点到点设备，“third-0-1.pcap”和“third-1-1.pcap”是无线网络WiFi的混杂跟踪。
  
