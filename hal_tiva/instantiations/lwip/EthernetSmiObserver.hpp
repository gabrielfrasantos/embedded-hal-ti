#ifndef HAL_TI_ETHERNET_SMI_OBSERVER_HPP
#define HAL_TI_ETHERNET_SMI_OBSERVER_HPP

#include "hal/interfaces/Ethernet.hpp"
#include "lwip/lwip_cpp/LightweightIpOverEthernet.hpp"

namespace instantiations
{
    class EthernetSmiObserver
        : public hal::EthernetSmiObserver
    {
    public:
        explicit EthernetSmiObserver(hal::EthernetSmi& ethernetSmi, hal::EthernetMac& ethernetMac, services::LightweightIpOverEthernetFactory& lightweightIpOverEthernetFactory);
        ~EthernetSmiObserver() override;

        void LinkUp(hal::LinkSpeed linkSpeed) override;
        void LinkDown() override;

    private:
        services::LightweightIpOverEthernetFactory& lightweightIpOverEthernetFactory;
        hal::EthernetMac& ethernetMac;
    };
}

#endif
