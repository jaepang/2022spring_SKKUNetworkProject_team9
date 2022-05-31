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

NS_LOG_COMPONENT_DEFINE("testing");

uint32_t accumPackets = 0;
static void
PacketCount(Ptr<const Packet> p)
{
    accumPackets += p->GetSize();
	NS_LOG_LOGIC(Simulator::Now ().GetSeconds () << "\t" << accumPackets);
}

int  main(int argc, char*argv[])
{
    LogComponentEnable("testing", LOG_LEVEL_ALL);
    // LogComponentEnable("CsmaNetDevice", LOG_LEVEL_ALL);
    // LogComponentEnable("BufBridgeNetDevice", LOG_LEVEL_DEBUG);
    // LogComponentEnable("BridgeNetDevice", LOG_LEVEL_ALL);
    // LogComponentEnable("PacketSink", LOG_LEVEL_ALL);
	// LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
    // LogComponentEnable("OnOffApplication", LOG_LEVEL_ALL);
    
    NodeContainer terminals;
    terminals.Create(3);

    NodeContainer bridgeSwitch;
    bridgeSwitch.Create(1);

	CsmaHelper csma;
	csma.SetChannelAttribute("DataRate", StringValue("2.5Mbps"));
	csma.SetChannelAttribute("Delay", StringValue("10us"));

	NetDeviceContainer csmaDevices;
    NetDeviceContainer switchDevices;

    for (int i = 0; i < 3; i++) {
        NetDeviceContainer link = csma.Install(NodeContainer(terminals.Get(i), bridgeSwitch));
        csmaDevices.Add(link.Get(0));
        switchDevices.Add(link.Get(1));
    }
    Ptr<Node> switchNode = bridgeSwitch.Get(0);
    BufBridgeHelper bridge; 
    // bridge.SetDeviceAttribute("EnableLearning", BooleanValue(false));
    bridge.Install(switchNode, switchDevices);

	InternetStackHelper stack;
	stack.Install(terminals);

	Ipv4AddressHelper ipv4;
	ipv4.SetBase("10.1.1.0", "255.255.255.0");
	Ipv4InterfaceContainer interfaces = ipv4.Assign(csmaDevices);

    uint16_t port = 9;

    // Application executing
    OnOffHelper onoff1("ns3::UdpSocketFactory", Address(InetSocketAddress(interfaces.GetAddress(0), port)));
    onoff1.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=5]"));
    onoff1.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
    onoff1.SetAttribute("DataRate", DataRateValue(250000));

    ApplicationContainer onoffApp1 = onoff1.Install(terminals.Get(2));
    onoffApp1.Start(Seconds(1.0));
    onoffApp1.Stop(Seconds(16.0));

    OnOffHelper onoff2("ns3::UdpSocketFactory", Address(InetSocketAddress(interfaces.GetAddress(1), port)));
    onoff2.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=5]"));
    onoff2.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
    onoff2.SetAttribute("DataRate", DataRateValue(250000));

    ApplicationContainer onoffApp2 = onoff2.Install(terminals.Get(2));
    onoffApp2.Start(Seconds(1.2));
    onoffApp2.Stop(Seconds(16.0));

    PacketSinkHelper sink1("ns3::UdpSocketFactory", Address(InetSocketAddress(Ipv4Address::GetAny(), port)));
    ApplicationContainer sinkApp1 = sink1.Install(terminals.Get(0));
    sinkApp1.Start(Seconds(1.0)); 

    PacketSinkHelper sink2("ns3::UdpSocketFactory", Address(InetSocketAddress(Ipv4Address::GetAny(), port)));
    ApplicationContainer sinkApp2 = sink2.Install(terminals.Get(1));
    sinkApp2.Start(Seconds(1.0)); 
    ////////////////////////////////////////////////////////////////////////

    csmaDevices.Get(2)->TraceConnectWithoutContext("MacTx", MakeCallback(&PacketCount));

    Simulator::Stop(Seconds(2));
	Simulator::Run();
	Simulator::Destroy();

    double throughput = accumPackets * 512 * 8 / (2*1000000.0);
    NS_LOG_LOGIC("Throughput: " << throughput << " Mbps");
	return 0;
}

