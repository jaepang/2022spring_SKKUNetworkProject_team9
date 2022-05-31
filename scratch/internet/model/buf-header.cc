// NetP Final

#include "ns3/log.h"
#include "buf-header.h"

namespace ns3
{
    NS_LOG_COMPONENT_DEFINE ("BufHeader");
    NS_OBJECT_ENSURE_REGISTERED (BufHeader);

    BufHeader::BufHeader()
     : m_model(0)
    {}

    uint16_t
    BufHeader::GetModelType(uint16_t type)
    {
        NS_LOG_FUNCTION (this);
        m_model = type;
        return m_model;
    }

    TypeId
    BufHeader::GetTypeId (void)
    {
    static TypeId tid = TypeId ("ns3::BufHeader")
        .SetParent<Header> ()
        .SetGroupName("Applications")
        .AddConstructor<BufHeader> ()
    ;
    return tid;
    }

    TypeId
    BufHeader::GetInstanceTypeId (void) const
    {
        return GetTypeId ();
    }

    void
    BufHeader::Print (std::ostream &os) const
    {
        NS_LOG_FUNCTION (this << &os);
        os << "(model=" << m_model << ")";
    }
    uint32_t
    BufHeader::GetSerializedSize (void) const
    {
        NS_LOG_FUNCTION (this);
        return 4+8;
    }

    void
    BufHeader::Serialize (Buffer::Iterator start) const
    {
        NS_LOG_FUNCTION (this << &start);
    }
    uint32_t
    BufHeader::Deserialize (Buffer::Iterator start)
    {
        NS_LOG_FUNCTION (this << &start);
        return GetSerializedSize ();
    }

} // namespace ns3
