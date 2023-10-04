#ifndef HAL_TI_ETHERNET_HPP
#define HAL_TI_ETHERNET_HPP

#include "hal_tiva/tiva/Ethernet.hpp"
#include "hal_tiva/instantiations/lwip/EthernetSmiObserver.hpp"
#include "lwip/lwip_cpp/LightweightIp.hpp"
#include "lwip/lwip_cpp/LightweightIpOverEthernet.hpp"
#include "services/network/LlmnrResponder.hpp"

namespace
{
    services::LightweightIpOverEthernetFactory::Config ConvertConfig(infra::BoundedString hostName)
    {
        services::LightweightIpOverEthernetFactory::Config config;
        config.hostName = hostName;
        return config;
    }
}

namespace main_
{
    template<std::size_t MaxListeners = 1, std::size_t MaxConnectors = 1, std::size_t MaxConnections = 5>
    struct Ethernet
    {
        Ethernet(hal::tiva::Ethernet::Leds leds, hal::MacAddress macAddress, infra::BoundedString hostName, hal::SynchronousRandomDataGenerator& randomDataGenerator, infra::CreatorBase<services::Stoppable, void(services::LightweightIp& lightweightIp)>& connectedCreator);

        services::LightweightIp::WithFixedAllocator<MaxListeners, MaxConnectors, MaxConnections> lightweightIp;
        services::LightweightIpOverEthernetFactory::Config lightweightIpConfig;
        services::LightweightIpOverEthernetFactory lightweightIpOverEthernetFactory;
        hal::tiva::Ethernet ethernet;
        instantiations::EthernetSmiObserver ethernetSmiObserver;
        infra::BoundedString hostName;

        struct Connected
            : public services::Stoppable
        {
            Connected(services::LightweightIp& lightweightIp, infra::BoundedConstString hostName, infra::CreatorBase<services::Stoppable, void(services::LightweightIp& lightweightIp)>* connectedCreator)
                : llmnrResponder(lightweightIp, lightweightIp, lightweightIp, hostName)
            {
                if (connectedCreator != nullptr)
                    connected.Emplace(*connectedCreator, lightweightIp);
            }

            void Stop(const infra::Function<void()>& onDone) override
            {
                if (connected != infra::none)
                    (*connected)->Stop(onDone);
                else
                    onDone();
            }

            services::LlmnrResponder llmnrResponder;
            infra::Optional<infra::ProxyCreator<services::Stoppable, void(services::LightweightIp& lightweightIp)>> connected;
        };

        infra::CreatorBase<services::Stoppable, void(services::LightweightIp& lightweightIp)>* connectedCreator = nullptr;
        infra::Creator<services::Stoppable, Connected, void(services::LightweightIp& lightweightIp)> connected{ [this](infra::Optional<Connected>& value, services::LightweightIp& lightweightIp)
            {
                value.Emplace(lightweightIp, hostName, connectedCreator);
            } };
    };

    template<std::size_t MaxListeners, std::size_t MaxConnectors, std::size_t MaxConnections>
    Ethernet<MaxListeners, MaxConnectors, MaxConnections>::Ethernet(hal::tiva::Ethernet::Leds leds, hal::MacAddress macAddress, infra::BoundedString hostName,
        hal::SynchronousRandomDataGenerator& randomDataGenerator, infra::CreatorBase<services::Stoppable, void(services::LightweightIp& lightweightIp)>& connectedCreator)
        : lightweightIp(randomDataGenerator, connected)
        , lightweightIpConfig(ConvertConfig(hostName))
        , lightweightIpOverEthernetFactory(macAddress, lightweightIpConfig)
        , ethernet(leds, hal::tiva::Ethernet::PhySelection::internal, hal::LinkSpeed::fullDuplex100MHz, lightweightIpOverEthernetFactory.MacAddress())
        , ethernetSmiObserver(ethernet, ethernet, lightweightIpOverEthernetFactory)
        , connectedCreator(&connectedCreator)
        , hostName(hostName)
    {}
}

#endif
