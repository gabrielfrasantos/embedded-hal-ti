#include "hal_tiva/tiva/Dma.hpp"
#include "infra/util/BitLogic.hpp"
#include "infra/util/ReallyAssert.hpp"

namespace hal::tiva
{
    namespace
    {
        constexpr static uint32_t CHCTL_DSTINC_M = 0xC0000000;  // Destination Address Increment
        constexpr static uint32_t CHCTL_DSTINC_8 = 0x00000000;  // Byte
        constexpr static uint32_t CHCTL_DSTINC_16 = 0x40000000;  // Half-word
        constexpr static uint32_t CHCTL_DSTINC_32 = 0x80000000;  // Word
        constexpr static uint32_t CHCTL_DSTINC_NONE = 0xC0000000;  // No increment
        constexpr static uint32_t CHCTL_DSTSIZE_M = 0x30000000;  // Destination Data Size
        constexpr static uint32_t CHCTL_DSTSIZE_8 = 0x00000000;  // Byte
        constexpr static uint32_t CHCTL_DSTSIZE_16 = 0x10000000;  // Half-word
        constexpr static uint32_t CHCTL_DSTSIZE_32 = 0x20000000;  // Word
        constexpr static uint32_t CHCTL_SRCINC_M = 0x0C000000;  // Source Address Increment
        constexpr static uint32_t CHCTL_SRCINC_8 = 0x00000000;  // Byte
        constexpr static uint32_t CHCTL_SRCINC_16 = 0x04000000;  // Half-word
        constexpr static uint32_t CHCTL_SRCINC_32 = 0x08000000;  // Word
        constexpr static uint32_t CHCTL_SRCINC_NONE = 0x0C000000;  // No increment
        constexpr static uint32_t CHCTL_SRCSIZE_M = 0x03000000;  // Source Data Size
        constexpr static uint32_t CHCTL_SRCSIZE_8 = 0x00000000;  // Byte
        constexpr static uint32_t CHCTL_SRCSIZE_16 = 0x01000000;  // Half-word
        constexpr static uint32_t CHCTL_SRCSIZE_32 = 0x02000000;  // Word
        constexpr static uint32_t CHCTL_DSTPROT0 = 0x00200000;  // Destination Privilege Access
        constexpr static uint32_t CHCTL_SRCPROT0 = 0x00040000;  // Source Privilege Access
        constexpr static uint32_t CHCTL_ARBSIZE_M = 0x0003C000;  // Arbitration Size
        constexpr static uint32_t CHCTL_ARBSIZE_1 = 0x00000000;  // 1 Transfer
        constexpr static uint32_t CHCTL_ARBSIZE_2 = 0x00004000;  // 2 Transfers
        constexpr static uint32_t CHCTL_ARBSIZE_4 = 0x00008000;  // 4 Transfers
        constexpr static uint32_t CHCTL_ARBSIZE_8 = 0x0000C000;  // 8 Transfers
        constexpr static uint32_t CHCTL_ARBSIZE_16 = 0x00010000;  // 16 Transfers
        constexpr static uint32_t CHCTL_ARBSIZE_32 = 0x00014000;  // 32 Transfers
        constexpr static uint32_t CHCTL_ARBSIZE_64 = 0x00018000;  // 64 Transfers
        constexpr static uint32_t CHCTL_ARBSIZE_128 = 0x0001C000;  // 128 Transfers
        constexpr static uint32_t CHCTL_ARBSIZE_256 =   0x00020000;  // 256 Transfers
        constexpr static uint32_t CHCTL_ARBSIZE_512 = 0x00024000;  // 512 Transfers
        constexpr static uint32_t CHCTL_ARBSIZE_1024 = 0x00028000;  // 1024 Transfers
        constexpr static uint32_t CHCTL_XFERSIZE_M = 0x00003FF0;  // Transfer Size (minus 1)
        constexpr static uint32_t CHCTL_NXTUSEBURST = 0x00000008;  // Next Useburst
        constexpr static uint32_t CHCTL_XFERMODE_M = 0x00000007;  // uDMA Transfer Mode
        constexpr static uint32_t CHCTL_XFERMODE_STOP = 0x00000000;  // Stop
        constexpr static uint32_t CHCTL_XFERMODE_BASIC = 0x00000001;  // Basic
        constexpr static uint32_t CHCTL_XFERMODE_AUTO = 0x00000002;  // Auto-Request
        constexpr static uint32_t CHCTL_XFERMODE_PINGPONG = 0x00000003;  // Ping-Pong
        constexpr static uint32_t CHCTL_XFERMODE_MEM_SG = 0x00000004;  // Memory Scatter-Gather
        constexpr static uint32_t CHCTL_XFERMODE_MEM_SGA = 0x00000005;  // Alternate Memory Scatter-Gather
        constexpr static uint32_t CHCTL_XFERMODE_PER_SG = 0x00000006;  // Peripheral Scatter-Gather
        constexpr static uint32_t CHCTL_XFERMODE_PER_SGA = 0x00000007;  // Alternate Peripheral Scatter-Gather
        constexpr static uint32_t CHCTL_XFERSIZE_S = 4;

        volatile uint32_t& HardwareRegister(uint32_t reg)
        {
            return *reinterpret_cast<volatile uint32_t *>(reg);
        }

        uint32_t ChannelControlMask()
        {
            return CHCTL_DSTINC_M | CHCTL_DSTSIZE_M | CHCTL_SRCINC_M | CHCTL_SRCSIZE_M | CHCTL_ARBSIZE_M | CHCTL_NXTUSEBURST;
        }

        template<class T>
        uint32_t ConvertEnumToUint32(const T& data, std::size_t offset)
        {
            return static_cast<uint32_t>(data) << offset;
        }

        uint32_t ChannelControlSet(const DmaChannel::Configuration& config)
        {
            auto data = ConvertEnumToUint32(config.destinationIncrement, 30);
            data |= ConvertEnumToUint32(config.dataSize, 28);
            data |= ConvertEnumToUint32(config.sourceIncrement, 26);
            data |= ConvertEnumToUint32(config.dataSize, 24);
            data |= ConvertEnumToUint32(config.arbitrationSize, 14);

            return data;
        }

        volatile void * GoToEndAddress(volatile void* address, const DmaChannel::Increment& increment, std::size_t size)
        {
            uint32_t bufferAddress = 0;

            if (increment != DmaChannel::Increment::none)
                bufferAddress = (size << static_cast<std::size_t>(increment)) - 1;

            return static_cast<volatile void *>(reinterpret_cast<volatile uint32_t*>(address) + bufferAddress);
        }
    }

    std::array<uint8_t, 1024> Dma::controlTable alignas(1024);

    Dma::Dma(const infra::Function<void()>& onError)
        : onError(onError)
    {
        EnableClock();
        Initialize();

        Register(UDMAERR_IRQn);
    }

    Dma::~Dma()
    {

    }

    void Dma::Invoke()
    {
        if (auto status = UDMA->ERRCLR; status != 0)
        {
            UDMA->ERRCLR = 1;
            this->onError();
        }
    }

    void Dma::EnableClock() const
    {
        SYSCTL->RCGCDMA |= 1;
    }

    void Dma::Initialize() const
    {
        auto startAddreses = reinterpret_cast<uint32_t>(&controlTable[0]);

        really_assert((startAddreses & ~0x3FF) == startAddreses);
        really_assert(startAddreses >= 0x2000000);

        UDMA->CFG |= 1;
        UDMA->CTLBASE = startAddreses;
    }

    DmaChannel::DmaChannel(const Channel& channel)
        : channel(channel)
    {
        really_assert(Dma::InstanceSet());

        SetChannel();
    }

    void DmaChannel::ConfigureChannel(const Configuration& configuration) const
    {
        DisableAllAttributes();

        auto controlArray = reinterpret_cast<DmaChannel::Control *>(UDMA->CTLBASE);
        auto index = channel.number + static_cast<uint32_t>(configuration.type);
        auto control = &controlArray[index];

        control->channelControl = (control->channelControl & ~ChannelControlMask()) | ChannelControlSet(configuration);
    }

    void DmaChannel::StartTransfer() const
    {
        UDMA->ENASET = 1 << channel.number;
    }

    DmaChannel::Transfer DmaChannel::Mode(Type type) const
    {
        auto controlArray = reinterpret_cast<DmaChannel::Control *>(UDMA->CTLBASE);
        auto index = channel.number + static_cast<uint32_t>(type);
        auto control = &controlArray[index];

        return static_cast<DmaChannel::Transfer>(control->channelControl & CHCTL_XFERMODE_M);
    }

    void DmaChannel::DisableAllAttributes() const
    {
        UDMA->USEBURSTCLR |= 1 << channel.number;
        UDMA->ALTCLR |= 1 << channel.number;
        UDMA->PRIOCLR |= 1 << channel.number;
        UDMA->REQMASKCLR |= 1 << channel.number;
    }

    void DmaChannel::SetChannel() const
    {
        const uint32_t channelMapSelect0 = 0x400FF510;

        auto mapEncodingRegister = channelMapSelect0 + static_cast<uint32_t>((channel.number / 8) * 4);
        auto shift = (channel.number % 8) * 4;

        HardwareRegister(mapEncodingRegister) = HardwareRegister(mapEncodingRegister) & ~(0xf << shift) | channel.mapping << shift;
    }

    void DmaChannel::ConfigureTransfer(Type type, Transfer transfer, volatile void * sourceAddress, volatile void * destinationAddress, std::size_t size) const
    {
        really_assert(size <= 1024);
        really_assert(reinterpret_cast<uint32_t>(sourceAddress) >= 0x20000000);
        really_assert(reinterpret_cast<uint32_t>(destinationAddress) >= 0x20000000);

        auto controlArray = reinterpret_cast<DmaChannel::Control *>(UDMA->CTLBASE);
        auto index = channel.number + static_cast<uint32_t>(type);
        auto control = &controlArray[index];

        UDMA->USEBURSTSET |= 1 << channel.number;

        auto localControl = (control->channelControl & ~(CHCTL_XFERSIZE_M | CHCTL_XFERMODE_M));
        localControl |= (size - 1) << 4;
        localControl |= static_cast<uint32_t>(transfer);

        control->sourceEndAddress = GoToEndAddress(sourceAddress, static_cast<Increment>((localControl & CHCTL_SRCINC_M) >> 26), size);
        control->destinationEndAddress = GoToEndAddress(destinationAddress, static_cast<Increment>((localControl & CHCTL_DSTINC_M) >> 30), size);
        control->channelControl = localControl;
    }
}