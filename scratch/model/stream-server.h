#ifndef STREAM_SERVER_H
#define STREAM_SERVER_H

#include "ns3/application.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/address.h"
#include "ns3/traced-callback.h"
#include "packet-loss-counter.h"

namespace ns3 {

class StreamServer : public Application
{
public:
    static TypeId GetTypeId(void);
    StreamServer();
    virtual ~StreamServer();

    uint32_t GetLost(void) const;
    uint64_t GetReceived(void) const;
    uint16_t GetPacketWindowSize() const;
    void SetPacketWindowSize(uint16_t size);

protected:
    virtual void DoDispose(void);

private:
    virtual void StartApplication(void);
    virtual void StopApplication(void);

    void HandleRead(Ptr<Socket> socket);
    void Send(Address ip);

    uint32_t    m_size;
    uint32_t m_count;
    Time m_interval;
    uint16_t m_port;
    Ptr<Socket> m_socket;
    Ptr<Socket> m_socket6;
    uint64_t m_received;
    PacketLossCounter m_lossCounter;
    uint16_t m_type;
    EventId m_sendEvent;
    uint32_t m_sent;
    TracedCallback<Ptr<const Packet>> m_rxTrace;
    TracedCallback<Ptr<const Packet>, const Address &, const Address &> m_rxTraceWithAddresses;
};

}

#endif