#include "stream-client-server-helper.h"
#include "ns3/stream-server.h"
#include "ns3/stream-client.h"
#include "ns3/uinteger.h"
#include "ns3/string.h"

namespace ns3 {

StreamServerHelper::StreamServerHelper ()
{
  m_factory.SetTypeId (StreamServer::GetTypeId ());
}

StreamServerHelper::StreamServerHelper (uint16_t port)
{
  m_factory.SetTypeId (StreamServer::GetTypeId ());
  SetAttribute ("Port", UintegerValue (port));
}

void
StreamServerHelper::SetAttribute (std::string name, const AttributeValue &value)
{
  m_factory.Set (name, value);
}

ApplicationContainer
StreamServerHelper::Install (NodeContainer c) const
{
  ApplicationContainer apps;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      apps.Add (InstallPriv (*i));
    }

  return apps;
}

ApplicationContainer
StreamServerHelper::Install (Ptr<Node> node) const
{
  return ApplicationContainer (InstallPriv (node));
}

ApplicationContainer
StreamServerHelper::Install (std::string nodeName) const
{
  Ptr<Node> node = Names::Find<Node> (nodeName);
  return ApplicationContainer (InstallPriv (node));
}

Ptr<StreamServer>
StreamServerHelper::GetServer (void)
{
  return m_server;
}

Ptr<Application>
StreamServerHelper::InstallPriv (Ptr<Node> node) const
{
  Ptr<Application> app = m_factory.Create<Application> ();
  node->AddApplication (app);

  return app;
}

StreamClientHelper::StreamClientHelper ()
{
  m_factory.SetTypeId (StreamClient::GetTypeId ());
}

StreamClientHelper::StreamClientHelper (Address address, uint16_t port)
{
  m_factory.SetTypeId (StreamClient::GetTypeId ());
  SetAttribute ("RemoteAddress", AddressValue (address));
  SetAttribute ("RemotePort", UintegerValue (port));
}

StreamClientHelper::StreamClientHelper (Address address)
{
  m_factory.SetTypeId (StreamClient::GetTypeId ());
  SetAttribute ("RemoteAddress", AddressValue (address));
}

void
StreamClientHelper::SetAttribute (std::string name, const AttributeValue &value)
{
  m_factory.Set (name, value);
}

ApplicationContainer
StreamClientHelper::Install (NodeContainer c) const
{
  ApplicationContainer apps;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      apps.Add (InstallPriv (*i));
    }

  return apps;
}

ApplicationContainer
StreamClientHelper::Install (Ptr<Node> node) const
{
  return ApplicationContainer (InstallPriv (node));
}

ApplicationContainer
StreamClientHelper::Install (std::string nodeName) const
{
  Ptr<Node> node = Names::Find<Node> (nodeName);
  return ApplicationContainer (InstallPriv (node));
}

Ptr<Application>
StreamClientHelper::InstallPriv (Ptr<Node> node) const
{
  Ptr<Application> app = m_factory.Create<Application> ();
  node->AddApplication (app);

  return app;
}

} // namespace ns3
