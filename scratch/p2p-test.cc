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
    static double bytes1, b1_time = 0;
    double time = Simulator::Now().GetSeconds();

    if(context == "Flow1") {
        bytes1 += p->GetSize();
        if (time - b1_time >= 0.1) {
            NS_LOG_UNCOND("1\t" << Simulator::Now().GetSeconds()  << "\t" << bytes1*8/1000000/(Simulator::Now().GetSeconds()-1));
            b1_time = time;
        }
    }
}

int  main(int argc, char*argv[])
{
    LogComponentEnable("TeamP", LOG_LEVEL_ALL);
    NodeContainer terminals;
    terminals.Create(2);

    NodeContainer bridgeSwitch;
    bridgeSwitch.Create(1);

	CsmaHelper csma;
	csma.SetChannelAttribute("DataRate", StringValue("5Mbps"));
	csma.SetChannelAttribute("Delay", StringValue("10us"));

	NetDeviceContainer csmaDevices;
    NetDeviceContainer switchDevices;

    for (int i = 0; i < 2; i++) {
        NetDeviceContainer link = csma.Install(NodeContainer(terminals.Get(i), bridgeSwitch));
        csmaDevices.Add(link.Get(0));
        switchDevices.Add(link.Get(1));
    }
    Ptr<Node> switchNode = bridgeSwitch.Get(0);
    BridgeHelper bridge; 
    bridge.Install(switchNode, switchDevices);

	InternetStackHelper stack;
	stack.Install(terminals);

	Ipv4AddressHelper ipv4;
	ipv4.SetBase("10.1.1.0", "255.255.255.0");
	Ipv4InterfaceContainer interfaces = ipv4.Assign(csmaDevices);

    uint16_t port = 9;


    ///////////////////////////// Flow1  ///////////////////////////////////  UDP or TCP
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
    ////////////////////////////////////////////////////////////////////////



    Simulator::Stop(Seconds(10));
	Simulator::Run();
	Simulator::Destroy();

	return 0;
}

