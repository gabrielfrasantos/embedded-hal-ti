#ifndef HAL_ETHERNET_TIVA_HPP
#define HAL_ETHERNET_TIVA_HPP

#include DEVICE_HEADER
#include "hal_tiva/cortex/InterruptCortex.hpp"
#include "hal/interfaces/Ethernet.hpp"
#include "hal/interfaces/MacAddress.hpp"
#include "hal_tiva/tiva/Gpio.hpp"
#include "infra/timer/Timer.hpp"
#include "infra/util/Sequencer.hpp"

namespace hal::tiva
{
    class Ethernet
        : public hal::EthernetSmi
        , public hal::EthernetMac
    {
    public:
        enum class PhySelection : uint32_t
        {
            internal = 0,
            externalMii = 0x80000000,
            externalRmii = 0xC0000000
        };

        Ethernet(PhySelection phySelection, hal::LinkSpeed linkSpeed, hal::MacAddress macAddress);
        ~Ethernet();

        void SendBuffer(infra::ConstByteRange data, bool last) override;
        void RetryAllocation() override;
        void AddMacAddressFilter(hal::MacAddress address) override;
        void RemoveMacAddressFilter(hal::MacAddress address) override;

        uint16_t PhyAddress() const override;

    private:
        struct EMACDescriptor;

        union EMACLink
        {
            EMACDescriptor *link; /* When DMA descriptors are used in chained mode, this field is used to provide a link to the next descriptor */
            void *buffer; /* When the DMA descriptors are unchained, this field may be used to point to a second buffer containing data for transmission or providing storage for a received frame */
        };

        struct EMACDescriptor
        {
            volatile uint32_t Desc0; /* contains various control and status bits depending upon whether the descriptor is in the transmit or receive queue */
            volatile uint32_t Desc1; /* contains information on the size of the buffer or buffers attached to the descriptor and various additional control bits */
            void *Desc2; /* The third descriptor word contains a pointer to the buffer containing data to transmit or into which received data should be written */
            EMACLink Desc3; /* contains either a pointer to the next descriptor in the ring or a pointer to a second data buffer */
            volatile uint32_t Desc4; /* transmit descriptors but used to report extended status in a receive descriptor */
            uint32_t reserved;
            volatile uint32_t Desc6; /* the low 32 bits of the 64-bit timestamp captured for transmitted or received data */
            volatile uint32_t Desc7; /* the high 32 bits of the 64-bit timestamp captured for transmitted or received data */
        };

        class ReceiveDescriptors
        {
        public:
            explicit ReceiveDescriptors(Ethernet& ethernetMac);

            void ReceivedFrame();
            void RequestReceiveBuffers();

        private:
            bool RequestReceiveBuffer();

        private:
            Ethernet& ethernetMac;

            std::array<EMACDescriptor, 4> descriptors;

            uint32_t receiveDescriptorAllocatedIndex = 0;
            uint32_t receiveDescriptorReceiveIndex = 0;
            uint32_t receivedFrameBuffers = 0;
            uint8_t receivedFramesAllocated = 0;
        };

        class SendDescriptors
        {
        public:
            explicit SendDescriptors(Ethernet& ethernetMac);

            void SendBuffer(infra::ConstByteRange data, bool last);
            void SentFrame();

        private:
            Ethernet& ethernetMac;

            std::array<EMACDescriptor, 8> descriptors;

            uint32_t sendDescriptorIndexFirst = 0;
            uint32_t sendDescriptorIndex = 0;
            bool sendFirst = true;
        };

        void InitilizeLeds();

        void EnableEMACClock() const;
        void ResetEMACClock() const;
        bool IsEMACReady() const;
        void EnableEPHYClock() const;
        void ResetEPHYClock() const;
        bool IsEPHYPresent() const;
        bool IsEPHYReady() const;

        void SelectPhy(PhySelection phySelection, hal::LinkSpeed linkSpeed) const;
        void ResetEthernetMac() const;
        void InitializeEthernetMac(uint32_t busConfig, uint32_t rxBurst, uint32_t txBurst, uint32_t descriptorSkipSize) const;
        void ConfigureEthernetMac(uint32_t configuration, uint32_t mode, uint32_t rxMaxFrameSize) const;
        void SetMacAddress(hal::MacAddress macAddress) const;
        uint16_t ReadExtendedPhy(uint8_t phyAddress, uint16_t registerAddress);
        uint16_t ReadPhy(uint8_t phyAddress, uint8_t registerAddress) const;
        void WritePhy(uint8_t phyAddress, uint8_t registerAddress, uint16_t data) const;
        void SetMacFilter(uint32_t options) const;
        void ConfigureTimestamp(uint32_t options, uint32_t subsecondClockIncrement, uint32_t adjust) const;
        void EnableTimestamp() const;
        void ClearInterruptPending(uint32_t mask) const;
        uint32_t GetInterruptStatus(bool mask) const;
        void EnableTxInterrupts() const;
        void EnableRxInterrupts() const;
        void EnableInterruptsSource(uint32_t options) const;
        void ProcessInterrupt(uint32_t status);
        void ProcessPhyInterrupt();
        void Interrupt();
        void GetEthernetMacConfiguration(uint32_t& config, uint32_t& mode, uint32_t& maxRxFrameSize);
        void SetEthernetMacConfiguration(uint32_t config, uint32_t mode, uint32_t maxRxFrameSize);
        void ConfigureLPITimers(bool config, uint16_t lsTimerInMs, uint16_t twTimerInMs);

        uint8_t phyId = 0;
        volatile bool EEELinkActive = false;
        DispatchedInterruptHandler interrupt;
        infra::Optional<ReceiveDescriptors> receiveDescriptors;
        infra::Optional<SendDescriptors> sendDescriptors;
    };
}

#endif
