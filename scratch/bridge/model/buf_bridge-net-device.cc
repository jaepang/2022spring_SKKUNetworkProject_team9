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
 *
 * Author: Gustavo Carneiro  <gjc@inescporto.pt>
 */
#include "buf_bridge-net-device.h"
#include "ns3/node.h"
#include "ns3/channel.h"
#include "ns3/packet.h"
#include "ns3/log.h"
#include "ns3/boolean.h"
#include "ns3/simulator.h"
#include "ns3/uinteger.h"
#include "ns3/buf-header.h"
namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("BufBridgeNetDevice");

NS_OBJECT_ENSURE_REGISTERED (BufBridgeNetDevice);


TypeId
BufBridgeNetDevice::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::BufBridgeNetDevice")
    .SetParent<NetDevice> ()
    .SetGroupName("Bridge")
    .AddConstructor<BufBridgeNetDevice> ()
    .AddAttribute ("Mtu", "The MAC-level Maximum Transmission Unit",
                   UintegerValue (1500),
                   MakeUintegerAccessor (&BufBridgeNetDevice::SetMtu,
                                         &BufBridgeNetDevice::GetMtu),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("EnableLearning",
                   "Enable the learning mode of the Learning Bridge",
                   BooleanValue (true),
                   MakeBooleanAccessor (&BufBridgeNetDevice::m_enableLearning),
                   MakeBooleanChecker ())
    .AddAttribute ("ExpirationTime",
                   "Time it takes for learned MAC state entry to expire.",
                   TimeValue (Seconds (300)),
                   MakeTimeAccessor (&BufBridgeNetDevice::m_expirationTime),
                   MakeTimeChecker ())
  ;
  return tid;
}


BufBridgeNetDevice::BufBridgeNetDevice ()
  : m_node (0),
    m_ifIndex (0)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_channel = CreateObject<BridgeChannel> ();
}

BufBridgeNetDevice::~BufBridgeNetDevice()
{
  NS_LOG_FUNCTION_NOARGS ();
}

void
BufBridgeNetDevice::DoDispose ()
{
  NS_LOG_FUNCTION_NOARGS ();
  for (std::vector< Ptr<NetDevice> >::iterator iter = m_ports.begin (); iter != m_ports.end (); iter++)
    {
      *iter = 0;
    }
  m_ports.clear ();
  m_channel = 0;
  m_node = 0;
  NetDevice::DoDispose ();
}

void
BufBridgeNetDevice::ReceiveFromDevice (Ptr<NetDevice> incomingPort, Ptr<const Packet> packet, uint16_t protocol,
                                    Address const &src, Address const &dst, PacketType packetType)
{
  NS_LOG_FUNCTION_NOARGS ();
  NS_LOG_DEBUG ("UID is " << packet->GetUid ());

  Mac48Address src48 = Mac48Address::ConvertFrom (src);
  Mac48Address dst48 = Mac48Address::ConvertFrom (dst);

  NS_LOG_DEBUG ("PACKET INFO " << src48 << "->" <<  dst48);
  if (!m_promiscRxCallback.IsNull ())
    {
      m_promiscRxCallback (this, packet, protocol, src, dst, packetType);
    }

  switch (packetType)
    {
    case PACKET_HOST:
    NS_LOG_DEBUG("PACKET_HOST" << packetType);
        if (dst48 == m_address)
        {
          Learn (src48, incomingPort);
          m_rxCallback (this, packet, protocol, src);
        }
        NS_LOG_DEBUG ("HOST" << src48 << "->" <<  dst48 << ":" << m_buffer.size());
      break;

    case PACKET_BROADCAST:
    case PACKET_MULTICAST:
        NS_LOG_DEBUG("PACKET_MULTICAST " << src48 << "->" << dst48);
        m_rxCallback (this, packet, protocol, src);
        ForwardBroadcast (incomingPort, packet, protocol, src48, dst48);
        break;

    case PACKET_OTHERHOST:
        NS_LOG_DEBUG("PACKET_OTHERHOST " << packetType);
        if (dst48 == m_address)
        {
          Learn (src48, incomingPort);
          m_rxCallback (this, packet, protocol, src);
        }
        else
        {   
            NS_LOG_DEBUG("Current Buffer");
            for (const auto& x : m_buffer) {
                NS_LOG_DEBUG("\t" << x.first);
            }
            // TODO
            Ptr<Packet> copyPkt = packet->Copy();

            uint16_t mode = copyPkt->GetSize();

            NS_LOG_INFO("MODE: " << mode);
            if (mode == 1024+28) {  // FROM SERVER
                if (m_buffer[src48].size() != 0) m_buffer[src48].pop_front();
                NS_LOG_DEBUG("Adding Packet ["<< packet->GetUid() <<"] to " << src48 << " Buffer SIZE: " << m_buffer[src48].size());
                m_buffer[src48].push_back(packet->Copy());

                for (Mac48Address it : m_list[src48]) { // Send it all registered client
                  ForwardReUnicast (incomingPort, m_buffer[src48].front(), protocol, src48, it);
                }

            }else if(mode==128+28) { // FROM CLIENT
                std::map<Mac48Address, std::deque<Ptr<Packet>> >::iterator iter = m_buffer.find (dst48);
                if (iter == m_buffer.end ()) {
                  NS_LOG_DEBUG("Adding New Buffer [" << src48 << "]");
                  std::deque<Ptr<Packet>> queue;
                  std::deque<Mac48Address> queue2;
                  m_buffer[dst48] = queue;
                  m_list[dst48] = queue2;
                }
                NS_LOG_DEBUG("Register Client");
                std::map<Mac48Address, std::deque<Mac48Address> >::iterator iter_n = m_list.find (src48);
                if (iter_n == m_list.end ()) {   // If not registered client, register
                  m_list[dst48].push_back(src48);
                  ForwardUnicast (incomingPort, packet, protocol, src48, dst48);
                }
            }else { // OTHER ELSE
              ForwardUnicast (incomingPort, packet, protocol, src48, dst48);
            }




            // std::map<Mac48Address, std::deque<Ptr<Packet>> >::iterator iter = m_buffer.find (src48);
            // if (iter == m_buffer.end ()) {
            //     NS_LOG_DEBUG("Adding New Buffer [" << src48 << "]");
            //     std::deque<Ptr<Packet>> queue;
            //     m_buffer[src48] = queue;
            //     NS_LOG_DEBUG("Adding Packet ["<< packet->GetUid() <<"] to " << src48 << " Buffer SIZE: " << m_buffer[src48].size());
            //     m_buffer[src48].push_back(packet->Copy());

            //     ForwardUnicast (incomingPort, packet, protocol, src48, dst48);

            // }else {
            //   if (m_buffer[src48].size() != 0) m_buffer[src48].pop_front();
            //   NS_LOG_DEBUG("Adding Packet ["<< packet->GetUid() <<"] to " << src48 << " Buffer SIZE: " << m_buffer[src48].size());
            //   m_buffer[src48].push_back(packet->Copy());
              

            //   iter = m_buffer.find (dst48);
            //   if (iter != m_buffer.end ()) {
            //       // If there is dst buf
            //       NS_LOG_DEBUG("Found Dst Buffer [" << dst48 << "] and Packet is " <<m_buffer[dst48].front()->GetUid() );
            //       NS_LOG_DEBUG("Sending Packet ["<< m_buffer[dst48].front()->GetUid() <<"] to " << src48);
            //       ForwardReUnicast (incomingPort, m_buffer[dst48].front(), protocol, src48, dst48);
            //       // Ptr<NetDevice> devicePort = GetBridgePort(1);

            //   }else {
            //       ForwardUnicast (incomingPort, packet, protocol, src48, dst48);
            //   }
            //   NS_LOG_DEBUG (src48 << "->" <<  dst48 << ":" << m_buffer.size());
            // }

        }
      break;
    }
}

void
BufBridgeNetDevice::ForwardUnicast (Ptr<NetDevice> incomingPort, Ptr<const Packet> packet,
                                 uint16_t protocol, Mac48Address src, Mac48Address dst)
{
  NS_LOG_FUNCTION_NOARGS ();
  NS_LOG_DEBUG ("LearningBridgeForward (incomingPort=" << incomingPort->GetInstanceTypeId ().GetName ()
                                                       << ", packet=" << packet << ", protocol="<<protocol
                                                       << ", src=" << src << ", dst=" << dst << ")");

  Learn (src, incomingPort);
  
  Ptr<NetDevice> outPort = GetLearnedState (dst);
  NS_LOG_DEBUG(incomingPort->GetAddress() << " VS " << outPort->GetAddress());
  if (outPort != NULL && outPort != incomingPort)
    {
      NS_LOG_LOGIC ("Learning bridge state says to use port `" << outPort->GetInstanceTypeId ().GetName () << "'");
      outPort->SendFrom (packet->Copy (), src, dst, protocol);
    }
  else
    {
      NS_LOG_LOGIC ("No learned state: send through all ports");
      for (std::vector< Ptr<NetDevice> >::iterator iter = m_ports.begin ();
           iter != m_ports.end (); iter++)
        {
          Ptr<NetDevice> port = *iter;
          if (port != incomingPort)
            {
              NS_LOG_LOGIC ("LearningBridgeForward (" << src << " => " << dst << "): " 
                                                      << incomingPort->GetInstanceTypeId ().GetName ()
                                                      << " --> " << port->GetInstanceTypeId ().GetName ()
                                                      << " (UID " << packet->GetUid () << ").");
              port->SendFrom (packet->Copy (), src, dst, protocol);
            }
        }
    }
}

void
BufBridgeNetDevice::ForwardReUnicast (Ptr<NetDevice> incomingPort, Ptr<const Packet> packet,
                                 uint16_t protocol, Mac48Address src, Mac48Address dst)
{
  NS_LOG_FUNCTION_NOARGS ();
  NS_LOG_DEBUG ("LearningBridgeForward (incomingPort=" << incomingPort->GetInstanceTypeId ().GetName ()
                                                       << ", packet=" << packet << ", protocol="<<protocol
                                                       << ", dst=" << dst << ")");

  Ptr<NetDevice> outPort = GetLearnedState (dst);

  if (outPort != NULL && outPort != incomingPort) {
    NS_LOG_DEBUG("Sending " << m_address << " " <<  dst);
    outPort->SendFrom (packet->Copy (), m_address, dst, protocol);
    
  } 
  else
    {
      NS_LOG_LOGIC ("No learned state: send through all ports");
      for (std::vector< Ptr<NetDevice> >::iterator iter = m_ports.begin ();
           iter != m_ports.end (); iter++)
        {
          Ptr<NetDevice> port = *iter;
          if (port != incomingPort)
            {
              NS_LOG_LOGIC ("LearningBridgeForward (" << src << " => " << dst << "): " 
                                                      << incomingPort->GetInstanceTypeId ().GetName ()
                                                      << " --> " << port->GetInstanceTypeId ().GetName ()
                                                      << " (UID " << packet->GetUid () << ").");
              port->SendFrom (packet->Copy (), src, dst, protocol);
            }
        }
    }
}
void
BufBridgeNetDevice::ForwardBroadcast (Ptr<NetDevice> incomingPort, Ptr<const Packet> packet,
                                   uint16_t protocol, Mac48Address src, Mac48Address dst)
{
  NS_LOG_FUNCTION_NOARGS ();
  NS_LOG_DEBUG ("LearningBridgeForward (incomingPort=" << incomingPort->GetInstanceTypeId ().GetName ()
                                                       << ", packet=" << packet << ", protocol="<<protocol
                                                       << ", src=" << src << ", dst=" << dst << ")");
  Learn (src, incomingPort);

  for (std::vector< Ptr<NetDevice> >::iterator iter = m_ports.begin ();
       iter != m_ports.end (); iter++)
    {
      Ptr<NetDevice> port = *iter;
      if (port != incomingPort)
        {
          NS_LOG_LOGIC ("LearningBridgeForward (" << src << " => " << dst << "): " 
                                                  << incomingPort->GetInstanceTypeId ().GetName ()
                                                  << " --> " << port->GetInstanceTypeId ().GetName ()
                                                  << " (UID " << packet->GetUid () << ").");
          port->SendFrom (packet->Copy (), src, dst, protocol);
        }
    }
}

void BufBridgeNetDevice::Learn (Mac48Address source, Ptr<NetDevice> port)
{
  NS_LOG_FUNCTION_NOARGS ();
  if (m_enableLearning)
    {
      LearnedState &state = m_learnState[source];
      state.associatedPort = port;
      state.expirationTime = Simulator::Now () + m_expirationTime;
    }
}

Ptr<NetDevice> BufBridgeNetDevice::GetLearnedState (Mac48Address source)
{
  NS_LOG_FUNCTION_NOARGS ();
  if (m_enableLearning)
    {
      Time now = Simulator::Now ();
      std::map<Mac48Address, LearnedState>::iterator iter = m_learnState.find (source);
      if (iter != m_learnState.end ())
        {
          LearnedState &state = iter->second;
          if (state.expirationTime > now)
            {
              return state.associatedPort;
            }
          else
            {
              m_learnState.erase (iter);
            }
        }
    }
  return NULL;
}

uint32_t
BufBridgeNetDevice::GetNBridgePorts (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return m_ports.size ();
}


Ptr<NetDevice>
BufBridgeNetDevice::GetBridgePort (uint32_t n) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return m_ports[n];
}

void 
BufBridgeNetDevice::AddBridgePort (Ptr<NetDevice> bridgePort)
{
  NS_LOG_FUNCTION_NOARGS ();
  NS_ASSERT (bridgePort != this);
  if (!Mac48Address::IsMatchingType (bridgePort->GetAddress ()))
    {
      NS_FATAL_ERROR ("Device does not support eui 48 addresses: cannot be added to bridge.");
    }
  if (!bridgePort->SupportsSendFrom ())
    {
      NS_FATAL_ERROR ("Device does not support SendFrom: cannot be added to bridge.");
    }
  if (m_address == Mac48Address ())
    {
      m_address = Mac48Address::ConvertFrom (bridgePort->GetAddress ());
    }

  NS_LOG_DEBUG ("RegisterProtocolHandler for " << bridgePort->GetInstanceTypeId ().GetName ());
  m_node->RegisterProtocolHandler (MakeCallback (&BufBridgeNetDevice::ReceiveFromDevice, this),
                                   0, bridgePort, true);
  m_ports.push_back (bridgePort);
  m_channel->AddChannel (bridgePort->GetChannel ());
}

void 
BufBridgeNetDevice::SetIfIndex (const uint32_t index)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_ifIndex = index;
}

uint32_t 
BufBridgeNetDevice::GetIfIndex (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return m_ifIndex;
}

Ptr<Channel> 
BufBridgeNetDevice::GetChannel (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return m_channel;
}

void
BufBridgeNetDevice::SetAddress (Address address)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_address = Mac48Address::ConvertFrom (address);
}

Address 
BufBridgeNetDevice::GetAddress (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return m_address;
}

bool 
BufBridgeNetDevice::SetMtu (const uint16_t mtu)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_mtu = mtu;
  return true;
}

uint16_t 
BufBridgeNetDevice::GetMtu (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return m_mtu;
}


bool 
BufBridgeNetDevice::IsLinkUp (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return true;
}


void 
BufBridgeNetDevice::AddLinkChangeCallback (Callback<void> callback)
{}


bool 
BufBridgeNetDevice::IsBroadcast (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return true;
}


Address
BufBridgeNetDevice::GetBroadcast (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return Mac48Address ("ff:ff:ff:ff:ff:ff");
}

bool
BufBridgeNetDevice::IsMulticast (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return true;
}

Address
BufBridgeNetDevice::GetMulticast (Ipv4Address multicastGroup) const
{
  NS_LOG_FUNCTION (this << multicastGroup);
  Mac48Address multicast = Mac48Address::GetMulticast (multicastGroup);
  return multicast;
}


bool 
BufBridgeNetDevice::IsPointToPoint (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return false;
}

bool 
BufBridgeNetDevice::IsBridge (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return true;
}


bool 
BufBridgeNetDevice::Send (Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber)
{
  NS_LOG_FUNCTION_NOARGS ();
  return SendFrom (packet, m_address, dest, protocolNumber);
}

bool 
BufBridgeNetDevice::SendFrom (Ptr<Packet> packet, const Address& src, const Address& dest, uint16_t protocolNumber)
{
  NS_LOG_FUNCTION_NOARGS ();
  Mac48Address dst = Mac48Address::ConvertFrom (dest); 

  // try to use the learned state if data is unicast
  if (!dst.IsGroup ())
    {
      Ptr<NetDevice> outPort = GetLearnedState (dst);
      if (outPort != NULL) 
        {
          outPort->SendFrom (packet, src, dest, protocolNumber);
          return true;
        }
    }

  // data was not unicast or no state has been learned for that mac
  // address => flood through all ports.
  Ptr<Packet> pktCopy;
  for (std::vector< Ptr<NetDevice> >::iterator iter = m_ports.begin ();
       iter != m_ports.end (); iter++)
    {
      pktCopy = packet->Copy ();
      Ptr<NetDevice> port = *iter;
      port->SendFrom (pktCopy, src, dest, protocolNumber);
    }

  return true;
}


Ptr<Node> 
BufBridgeNetDevice::GetNode (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return m_node;
}


void 
BufBridgeNetDevice::SetNode (Ptr<Node> node)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_node = node;
}


bool 
BufBridgeNetDevice::NeedsArp (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return true;
}


void 
BufBridgeNetDevice::SetReceiveCallback (NetDevice::ReceiveCallback cb)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_rxCallback = cb;
}

void 
BufBridgeNetDevice::SetPromiscReceiveCallback (NetDevice::PromiscReceiveCallback cb)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_promiscRxCallback = cb;
}

bool
BufBridgeNetDevice::SupportsSendFrom () const
{
  NS_LOG_FUNCTION_NOARGS ();
  return true;
}

Address BufBridgeNetDevice::GetMulticast (Ipv6Address addr) const
{
  NS_LOG_FUNCTION (this << addr);
  return Mac48Address::GetMulticast (addr);
}

} // namespace ns3
