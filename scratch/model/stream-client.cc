// NetP Final

#include "ns3/log.h"
#include "ns3/ipv4-address.h"
#include "ns3/nstime.h"
#include "ns3/inet-socket-address.h"
#include "ns3/inet6-socket-address.h"
#include "ns3/socket.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "stream-client.h"
#include "seq-ts-header.h"
#include "ns3/buf-header.h"
#include <cstdlib>
#include <cstdio>

namespace ns3{
NS_LOG_COMPONENT_DEFINE ("StreamClient");

NS_OBJECT_ENSURE_REGISTERED(StreamClient);

TypeId
StreamClient::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::StreamClient")
    .SetParent<Application> ()
    .SetGroupName("Applications")
    .AddConstructor<StreamClient> ()
    .AddAttribute ("MaxPackets",
                   "The maximum number of packets the application will send",
                   UintegerValue (100),
                   MakeUintegerAccessor (&StreamClient::m_count),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("Interval",
                   "The time to wait between packets", TimeValue (Seconds (1.0)),
                   MakeTimeAccessor (&StreamClient::m_interval),
                   MakeTimeChecker ())
    .AddAttribute ("RemoteAddress",
                   "The destination Address of the outbound packets",
                   AddressValue (),
                   MakeAddressAccessor (&StreamClient::m_peerAddress),
                   MakeAddressChecker ())
    .AddAttribute ("RemotePort", "The destination port of the outbound packets",
                   UintegerValue (100),
                   MakeUintegerAccessor (&StreamClient::m_peerPort),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("PacketSize",
                   "Size of packets generated. The minimum packet size is 12 bytes which is the size of the header carrying the sequence number and the time stamp.",
                   UintegerValue (1024),
                   MakeUintegerAccessor (&StreamClient::m_size),
                   MakeUintegerChecker<uint32_t> (12,65507))
  ;
  return tid;
}

StreamClient::StreamClient ()
{
  NS_LOG_FUNCTION (this);
  m_sent = 0;
  m_socket = 0;
  m_sendEvent = EventId ();
  m_type = 1;
}

StreamClient::~StreamClient ()
{
  NS_LOG_FUNCTION (this);
}

void
StreamClient::SetRemote (Address ip, uint16_t port)
{
  NS_LOG_FUNCTION (this << ip << port);
  m_peerAddress = ip;
  m_peerPort = port;
}

void
StreamClient::SetRemote (Address addr)
{
  NS_LOG_FUNCTION (this << addr);
  m_peerAddress = addr;
}

void
StreamClient::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  Application::DoDispose ();
}

void
StreamClient::StartApplication (void)
{
  NS_LOG_FUNCTION (this);

  if (m_socket == 0)
    {
      TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
      m_socket = Socket::CreateSocket (GetNode (), tid);
      if (Ipv4Address::IsMatchingType(m_peerAddress) == true)
        {
          if (m_socket->Bind () == -1)
            {
              NS_FATAL_ERROR ("Failed to bind socket");
            }
          m_socket->Connect (InetSocketAddress (Ipv4Address::ConvertFrom(m_peerAddress), m_peerPort));
        }
      else if (Ipv6Address::IsMatchingType(m_peerAddress) == true)
        {
          if (m_socket->Bind6 () == -1)
            {
              NS_FATAL_ERROR ("Failed to bind socket");
            }
          m_socket->Connect (Inet6SocketAddress (Ipv6Address::ConvertFrom(m_peerAddress), m_peerPort));
        }
      else if (InetSocketAddress::IsMatchingType (m_peerAddress) == true)
        {
          if (m_socket->Bind () == -1)
            {
              NS_FATAL_ERROR ("Failed to bind socket");
            }
          m_socket->Connect (m_peerAddress);
        }
      else if (Inet6SocketAddress::IsMatchingType (m_peerAddress) == true)
        {
          if (m_socket->Bind6 () == -1)
            {
              NS_FATAL_ERROR ("Failed to bind socket");
            }
          m_socket->Connect (m_peerAddress);
        }
      else
        {
          NS_ASSERT_MSG (false, "Incompatible address type: " << m_peerAddress);
        }
    }

  m_socket->SetRecvCallback (MakeNullCallback<void, Ptr<Socket> > ());
  m_socket->SetAllowBroadcast (true);
  m_sendEvent = Simulator::Schedule (Seconds (0.0), &StreamClient::Send, this);
}

void
StreamClient::StopApplication (void)
{
  NS_LOG_FUNCTION (this);
  Simulator::Cancel (m_sendEvent);
}

void
StreamClient::Send (void)
{
  NS_LOG_FUNCTION (this);
  NS_ASSERT (m_sendEvent.IsExpired ());

  BufHeader buf;
  SeqTsHeader seqTs;
  seqTs.SetSeq (m_sent);
  Ptr<Packet> p = Create<Packet> (m_size-(8+4)-(8+4)); // 8+4 : the size of the seqTs header
  p->AddHeader (seqTs);
  p->AddHeader (buf);

  std::stringstream peerAddressStringStream;
  if (Ipv4Address::IsMatchingType (m_peerAddress))
    {
      peerAddressStringStream << Ipv4Address::ConvertFrom (m_peerAddress);
    }
  else if (Ipv6Address::IsMatchingType (m_peerAddress))
    {
      peerAddressStringStream << Ipv6Address::ConvertFrom (m_peerAddress);
    }

  if ((m_socket->Send (p)) >= 0)
    {
      ++m_sent;
      NS_LOG_INFO ("TraceDelay TX " << m_size << " bytes to "
                                    << peerAddressStringStream.str () << " Uid: "
                                    << p->GetUid () << " Time: "
                                    << (Simulator::Now ()).GetSeconds ());

    }
  else
    {
      NS_LOG_INFO ("Error while sending " << m_size << " bytes to "
                                          << peerAddressStringStream.str ());
    }

  if (m_sent < m_count)
    {
      m_sendEvent = Simulator::Schedule (m_interval, &StreamClient::Send, this);
    }
}

} // Namespace ns3s