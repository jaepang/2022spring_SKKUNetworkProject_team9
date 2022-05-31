#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/packet-sink.h"
#include "ns3/csma-module.h"
#include "ns3/bridge-module.h"
#include <iostream>

using namespace ns3;
using namespace std;

NS_LOG_COMPONENT_DEFINE("TeamP");

static void
Rxtime(std::string context, Ptr<const Packet> p, const Address &a) {
<<<<<<< HEAD
    static double bytes1, b1_time = 0;
=======
    static double bytes1, bytes2, b1_time, b2_time = 0;
>>>>>>> 69a773da9c124aa4023b1f5409e2f1f0c051e9cd
    double time = Simulator::Now().GetSeconds();

    if(context == "Flow1") {
        bytes1 += p->GetSize();
        if (time - b1_time >= 0.1) {
            NS_LOG_UNCOND("1\t" << Simulator::Now().GetSeconds()  << "\t" << bytes1*8/1000000/(Simulator::Now().GetSeconds()-1));
            b1_time = time;
        }
    }
<<<<<<< HEAD
=======
    else if(context == "Flow2") {
        bytes2 += p->GetSize();
        if (time - b2_time >= 0.1) {
            NS_LOG_UNCOND("2\t" << Simulator::Now().GetSeconds()  << "\t" << bytes1*8/1000000/(Simulator::Now().GetSeconds()-1));
            b2_time = time;
        }
    }
>>>>>>> 69a773da9c124aa4023b1f5409e2f1f0c051e9cd
}

int  main(int argc, char*argv[])
{
    LogComponentEnable("TeamP", LOG_LEVEL_ALL);
<<<<<<< HEAD
    NodeContainer terminals;
    terminals.Create(2);
=======
    LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable("BufBridgeNetDevice", LOG_LEVEL_ALL);
    LogComponentEnable("BridgeNetDevice", LOG_LEVEL_ALL);
	LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
    //LogComponentEnable("OnOffApplication", LOG_LEVEL_ALL);
    NodeContainer terminals;
    terminals.Create(3);
>>>>>>> 69a773da9c124aa4023b1f5409e2f1f0c051e9cd

    NodeContainer bridgeSwitch;
    bridgeSwitch.Create(1);

	CsmaHelper csma;
	csma.SetChannelAttribute("DataRate", StringValue("5Mbps"));
	csma.SetChannelAttribute("Delay", StringValue("10us"));

	NetDeviceContainer csmaDevices;
    NetDeviceContainer switchDevices;

<<<<<<< HEAD
    for (int i = 0; i < 2; i++) {
=======
    for (int i = 0; i < 3; i++) {
>>>>>>> 69a773da9c124aa4023b1f5409e2f1f0c051e9cd
        NetDeviceContainer link = csma.Install(NodeContainer(terminals.Get(i), bridgeSwitch));
        csmaDevices.Add(link.Get(0));
        switchDevices.Add(link.Get(1));
    }
    Ptr<Node> switchNode = bridgeSwitch.Get(0);
<<<<<<< HEAD
    BridgeHelper bridge; 
=======
    BufBridgeHelper bridge; 
>>>>>>> 69a773da9c124aa4023b1f5409e2f1f0c051e9cd
    bridge.Install(switchNode, switchDevices);

	InternetStackHelper stack;
	stack.Install(terminals);

	Ipv4AddressHelper ipv4;
	ipv4.SetBase("10.1.1.0", "255.255.255.0");
	Ipv4InterfaceContainer interfaces = ipv4.Assign(csmaDevices);

    uint16_t port = 9;


    ///////////////////////////// Flow1  ///////////////////////////////////  UDP or TCP
<<<<<<< HEAD
    OnOffHelper onoff("ns3::UdpSocketFactory", Address(InetSocketAddress(Ipv4Address("10.1.1.2"), port)));
    onoff.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=16]"));
    onoff.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
    onoff.SetAttribute("DataRate", DataRateValue(250000));

    ApplicationContainer onoffApp1 = onoff.Install(terminals.Get(0));
    onoffApp1.Start(Seconds(1.0));
    onoffApp1.Stop(Seconds(16.0));

    PacketSinkHelper sink("ns3::UdpSocketFactory", Address(InetSocketAddress(Ipv4Address::GetAny(), port)));
    ApplicationContainer sinkApp1 = sink.Install(terminals.Get(1));
    sinkApp1.Start(Seconds(1.0)); 
    sinkApp1.Get(0)->TraceConnect("Rx", "Flow1", MakeCallback(&Rxtime));  
=======
	UdpEchoClientHelper echoClient(interfaces.GetAddress(2), port);
	echoClient.SetAttribute("MaxPackets", UintegerValue(1500));
	echoClient.SetAttribute("Interval", TimeValue(Seconds(0.5)));
	echoClient.SetAttribute("PacketSize", UintegerValue(1050));
    NS_LOG_INFO("echoClient Setted");

    ApplicationContainer echoClientpp1 = echoClient.Install(terminals.Get(0));
    echoClientpp1.Start(Seconds(0.5));
    echoClientpp1.Stop(Seconds(16.0));

    ApplicationContainer echoClientpp2 = echoClient.Install(terminals.Get(1));
    echoClientpp2.Start(Seconds(1.3));
    echoClientpp2.Stop(Seconds(16.0));

	// UdpEchoServerHelper echoServer(9);
	// ApplicationContainer serverApps(echoServer.Install(terminals.Get(2)));
	// serverApps.Start(Seconds(0));
	// serverApps.Stop(Seconds(16.0));

    OnOffHelper onoff2("ns3::UdpSocketFactory", Address(InetSocketAddress(Ipv4Address("10.1.1.1"), port)));
    onoff2.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=0.5]"));
    onoff2.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0.5]"));
    onoff2.SetAttribute("DataRate", DataRateValue(250000));

    ApplicationContainer onoffApp2 = onoff2.Install(terminals.Get(2));
    onoffApp2.Start(Seconds(1.0));
    onoffApp2.Stop(Seconds(16.0));

    // PacketSinkHelper sink1("ns3::UdpSocketFactory", Address(InetSocketAddress(Ipv4Address::GetAny(), port)));
    // ApplicationContainer sinkApp1 = sink1.Install(terminals.Get(0));
    // sinkApp1.Start(Seconds(1.0)); 
    // sinkApp1.Get(0)->TraceConnect("Rx", "Flow1", MakeCallback(&Rxtime));  
    
    PacketSinkHelper sink2("ns3::UdpSocketFactory", Address(InetSocketAddress(Ipv4Address::GetAny(), port)));
    ApplicationContainer sinkApp2 = sink2.Install(terminals.Get(0));
    sinkApp2.Start(Seconds(1.0)); 
    sinkApp2.Get(0)->TraceConnect("Rx", "Flow2", MakeCallback(&Rxtime));
>>>>>>> 69a773da9c124aa4023b1f5409e2f1f0c051e9cd
    ////////////////////////////////////////////////////////////////////////



<<<<<<< HEAD
    Simulator::Stop(Seconds(10));
=======
    Simulator::Stop(Seconds(5));
>>>>>>> 69a773da9c124aa4023b1f5409e2f1f0c051e9cd
	Simulator::Run();
	Simulator::Destroy();

	return 0;
}

