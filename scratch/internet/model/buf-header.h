#ifndef BUF_HEADER_H
#define BUF_HEADER_H

#include "ns3/header.h"

namespace ns3
{
class BufHeader : public Header
{
public:
    BufHeader();
    virtual ~BufHeader();

    static TypeId GetTypeId(void);
    uint16_t GetModelType(uint16_t t);

    virtual TypeId GetInstanceTypeId (void) const;
    virtual void Print (std::ostream &os) const;
    virtual uint32_t GetSerializedSize (void) const;
    virtual void Serialize (Buffer::Iterator start) const;
    virtual uint32_t Deserialize (Buffer::Iterator start);
private:
    uint16_t m_model; // 1: client, 2: server
};
} // namespace ns3

#endif