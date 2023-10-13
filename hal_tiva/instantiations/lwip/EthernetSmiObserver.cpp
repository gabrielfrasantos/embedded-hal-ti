#include "hal_tiva/instantiations/lwip/EthernetSmiObserver.hpp"

namespace instantiations
{
    EthernetSmiObserver::EthernetSmiObserver(hal::EthernetSmi& ethernetSmi, hal::EthernetMac& ethernetMac, services::LightweightIpOverEthernetFactory& lightweightIpOverEthernetFactory)
        : hal::EthernetSmiObserver(ethernetSmi)
        , lightweightIpOverEthernetFactory(lightweightIpOverEthernetFactory)
        , ethernetMac(ethernetMac)
    {}

    EthernetSmiObserver::~EthernetSmiObserver()
    {
        lightweightIpOverEthernetFactory.Destroy();
    }

    void EthernetSmiObserver::LinkUp(hal::LinkSpeed linkSpeed)
    {
        lightweightIpOverEthernetFactory.Create(ethernetMac);
    }

    void EthernetSmiObserver::LinkDown()
    {
        lightweightIpOverEthernetFactory.Destroy();
    }
}
