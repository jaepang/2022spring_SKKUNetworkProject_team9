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

NS_LOG_COMPONENT_DEFINE("teamProj");

uint32_t accumPackets = 0;
static void
PacketCount(Ptr<const Packet> p)
{
    accumPackets += p->GetSize();
	NS_LOG_LOGIC(Simulator::Now ().GetSeconds () << "\t" << accumPackets);
}

int  main(int argc, char*argv[])
{
    LogComponentEnable("teamProj", LOG_LEVEL_ALL);
    // LogComponentEnable("StreamClient", LOG_LEVEL_ALL);
    // LogComponentEnable("StreamServer", LOG_LEVEL_ALL);
    // LogComponentEnable("BufBridgeNetDevice", LOG_LEVEL_ALL);
    // LogComponentEnable("SeqTsHeader", LOG_LEVEL_ALL);
    // LogComponentEnable("PacketSink", LOG_LEVEL_ALL);

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
    bridge.Install(switchNode, switchDevices);

	InternetStackHelper stack;
	stack.Install(terminals);

	Ipv4AddressHelper ipv4;
	ipv4.SetBase("10.1.1.0", "255.255.255.0");
	Ipv4InterfaceContainer interfaces = ipv4.Assign(csmaDevices);

    uint16_t port = 9;

    // Application executing
    StreamClientHelper clientHelper0(interfaces.GetAddress(2), port);
    clientHelper0.SetAttribute("MaxPackets", UintegerValue(100));
    clientHelper0.SetAttribute("Interval", TimeValue(Seconds(0.5)));
    clientHelper0.SetAttribute("PacketSize", UintegerValue(128));
    ApplicationContainer clientApp0 = clientHelper0.Install(terminals.Get(0));
    clientApp0.Start(Seconds(0.8)); 

    StreamClientHelper clientHelper1(interfaces.GetAddress(2), port);
    clientHelper1.SetAttribute("MaxPackets", UintegerValue(100));
    clientHelper1.SetAttribute("Interval", TimeValue(Seconds(0.5)));
    clientHelper1.SetAttribute("PacketSize", UintegerValue(128));
    ApplicationContainer clientApp1 = clientHelper1.Install(terminals.Get(1));
    clientApp1.Start(Seconds(0.9)); 


    StreamServerHelper streamerHelper1(9);
    streamerHelper1.SetAttribute("MaxPackets", UintegerValue(100));
    streamerHelper1.SetAttribute("Interval", TimeValue(Seconds(0.01)));
    streamerHelper1.SetAttribute("PacketSize", UintegerValue(1024));
    ApplicationContainer streamerApp1 = streamerHelper1.Install(terminals.Get(2));
    streamerApp1.Start(Seconds(0.0)); 
    ////////////////////////////////////////////////////////////////////////

    csmaDevices.Get(2)->TraceConnectWithoutContext("MacTx", MakeCallback(&PacketCount));

    Simulator::Stop(Seconds(2));
	Simulator::Run();
	Simulator::Destroy();

    double throughput = accumPackets * 512 * 8 / (1000000.0);
    NS_LOG_LOGIC("Throughput: " << throughput << " Mbps");
	return 0;
}

