// NetP Final

#ifndef STREAM_CLIENT_H
#define STREAM_CLIENT_H

#include "ns3/application.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/ipv4-address.h"

namespace ns3 {

class Socket;
class Packet;

class StreamClient : public Application
{
public:
    static TypeId GetTypeId(void);
    StreamClient();
    virtual ~StreamClient();

    void SetRemote(Address ip, uint16_t port);
    void SetRemote(Address addr);

protected:
    virtual void DoDispose(void);

private:
    virtual void StartApplication(void);
    virtual void StopApplication(void);

    void Send(void);

    Ptr<Socket> m_socket;
    Address     m_peerAddress;
    uint16_t    m_peerPort;
    uint16_t    m_type;

    uint32_t    m_count;
    Time        m_interval;
    uint32_t    m_size;
    uint32_t    mode;
    uint32_t    m_sent;
    EventId     m_sendEvent;
};

}

#endif