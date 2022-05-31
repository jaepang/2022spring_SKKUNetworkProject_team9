#ifndef STREAM_CLIENT_SERVER_HELPER
#define STREAM_CLIENT_SERVER_HELPER

#include <stdint.h>
#include "ns3/application-container.h"
#include "ns3/node-container.h"
#include "ns3/object-factory.h"
#include "ns3/ipv4-address.h"
#include "ns3/stream-server.h"
#include "ns3/stream-client.h"

namespace ns3
{
    
class StreamServerHelper
{
public:
    StreamServerHelper();
    StreamServerHelper(uint16_t port);

    void SetAttribute(std::string name, const AttributeValue &value);
    ApplicationContainer Install (NodeContainer c) const;
    ApplicationContainer Install (Ptr<Node> node) const;
    ApplicationContainer Install (std::string nodeName) const;
    Ptr<StreamServer> GetServer(void);

private:
    ObjectFactory m_factory;
    Ptr<Application> InstallPriv (Ptr<Node> node) const;
};


class StreamClientHelper
{
    StreamClientHelper();
    StreamClientHelper(Address ip, uint16_t port);
    StreamClientHelper(Address addr);

    void SetAttribute(std::string name, const AttributeValue &value);
    ApplicationContainer Install (NodeContainer c) const;
    ApplicationContainer Install (Ptr<Node> node) const;
    ApplicationContainer Install (std::string nodeName) const;

private:
    ObjectFactory m_factory;
    Ptr<Application> InstallPriv (Ptr<Node> node) const;
};

} // namespace ns3

#endif