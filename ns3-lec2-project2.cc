/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/constant-velocity-mobility-model.h"
#include "ns3/mobility-helper.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"

// Default Network Topology
//
//   Wifi_2 10.1.3.0
//                 AP
//  *    *    *    *
//  |    |    |    |    10.1.1.0
// n5   n6   n7   n0 -------------- n1   n2   n3   n4
//                   point-to-point  |    |    |    |
//                                   *    *    *    *
//                                   AP  wifi_1 10.1.2.0

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("ThirdScriptExample");  //定义记录组件 


int 
main (int argc, char *argv[])
{
  bool verbose = true; //记录组件boot or not
  uint32_t nWifi1 = 4; //wifi1节点数量
  uint32_t nWifi2 = 4; //wifi2节点数量

  CommandLine cmd;
  cmd.AddValue ("nWifi1", "Number of wifi STA nodes/devices", nWifi1);
  cmd.AddValue ("nWifi2", "Number of wifi STA devices", nWifi2);
  cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose);

  cmd.Parse (argc,argv);

  if (nWifi1 > 18 || nWifi2>18)
    {
      std::cout << "Number of wifi nodes specified exceeds the mobility bounding box" << std::endl;
      exit (1);
    }

  if (verbose)
    {
      LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
      LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }

  NodeContainer p2pNodes;  //创建2个节点，p2p链路两端
  p2pNodes.Create (2);

  //创建信道，设置信道参数，在设备安装到节点上
  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer p2pDevices;
  p2pDevices = pointToPoint.Install (p2pNodes);

  
  /*wifi_1 config*/
  NodeContainer wifi1StaNodes;
  wifi1StaNodes.Create (nWifi1);
  NodeContainer wifi1ApNode = p2pNodes.Get (1);

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

  MobilityHelper mobility;

  mobility.SetMobilityModel ("ns3::ConstantVelocityMobilityModel");
  mobility.Install (wifi1StaNodes);
  NodeContainer::Iterator temp = wifi1StaNodes.Begin();
  for (;temp != wifi1StaNodes.End();temp++)
  {
  	(*temp)->GetObject<ConstantVelocityMobilityModel>()->SetVelocity(Vector(0.01,0,0));
  }
  
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (wifi1ApNode);

  /*wifi2 config*/
  NodeContainer wifi2StaNodes;
  wifi2StaNodes.Create (nWifi2);
  NodeContainer wifi2ApNode = p2pNodes.Get (0);

  //创建无线设备于无线节点之间的互联通道，并将通道对象与物理层对象关联
  //确保所有物理层对象使用相同的底层信道，即无线信道
  YansWifiChannelHelper channel2 = YansWifiChannelHelper::Default ();
  YansWifiPhyHelper phy2 = YansWifiPhyHelper::Default ();
  phy2.SetChannel (channel2.Create ());

  //配置速率控制算法，AARF算法
  WifiHelper wifi2 = WifiHelper::Default ();
  wifi2.SetRemoteStationManager ("ns3::AarfWifiManager");

  NqosWifiMacHelper mac2 = NqosWifiMacHelper::Default ();

  Ssid ssid2 = Ssid ("wifi_2");
  mac2.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssid2),
               "ActiveProbing", BooleanValue (false));

  NetDeviceContainer sta2Devices;
  sta2Devices = wifi2.Install (phy2, mac2, wifi2StaNodes);

  mac2.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid2));

  NetDeviceContainer ap2Devices;
  ap2Devices = wifi2.Install (phy2, mac2, wifi2ApNode);



  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (wifi2StaNodes);

  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (wifi2ApNode);

  
  //已经创建了节点，设备，信道和移动模型，接下来配置协议栈
  InternetStackHelper stack;
  stack.Install (wifi1ApNode);
  stack.Install (wifi2ApNode);
  stack.Install (wifi1StaNodes);
  stack.Install (wifi2StaNodes);
  
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
//放置echo服务端程序在最右边的csma节点,端口为9
  UdpEchoServerHelper echoServer (9);

  ApplicationContainer serverApps = echoServer.Install (wifi2StaNodes.Get(0));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));
//回显客户端放在最后的STA节点，指向wifi网络的服务器，上面的节点地址，端口为9
  UdpEchoClientHelper echoClient (wifi2StaInterfaces.GetAddress(0), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (2));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps = 
  echoClient.Install (wifi1StaNodes);
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (10.0));

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  Simulator::Stop (Seconds (10.0));

  pointToPoint.EnablePcapAll ("third");
  phy.EnablePcap ("third", ap2Devices.Get (0));
  phy.EnablePcap ("third", ap1Devices.Get (0));

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
