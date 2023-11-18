#ifndef HAL_DMA_STM_HPP
#define HAL_DMA_STM_HPP

#include "infra/util/InterfaceConnector.hpp"
#include "infra/util/Optional.hpp"
#include DEVICE_HEADER
#include "infra/util/Function.hpp"
#include "hal_tiva/cortex/InterruptCortex.hpp"
#include <cstdint>

namespace hal::tiva
{
    class Dma
        : public infra::InterfaceConnector<Dma>
        , private InterruptHandler
    {
    public:
        explicit Dma(const infra::Function<void()>& onError);
        virtual ~Dma();
        void Invoke() override;

    private:
        void EnableClock() const;
        void Initialize() const;

        infra::Function<void()> onError;
        static std::array<uint8_t, 1024> controlTable alignas(1024);
    };

    class DmaChannel
    {
    public:
        enum class Type : uint8_t
        {
            primary = 0x00,
            secondary = 0x20,
        };

        enum class Priority : uint8_t
        {
            standard = 0,
            high,
        };

        enum class Transfer : uint8_t
        {
            stop,
            basic,
            automatic,
            pingPong,
        };

        enum class DataSize : uint8_t
        {
            _8_bits,
            _16_bits,
            _32_bits,
        };

        enum class Increment : uint8_t
        {
            _8_bits,
            _16_bits,
            _32_bits,
            none,
        };

        enum class ArbitrationSize : uint8_t
        {
            _1_item,
            _2_items,
            _4_items,
            _8_items,
            _16_items,
            _32_items,
            _64_items,
            _128_items,
            _256_items,
            _512_items,
            _1024_items,
        };

        struct Configuration
        {
            Type type;
            Increment sourceIncrement;
            Increment destinationIncrement;
            DataSize dataSize;
            ArbitrationSize arbitrationSize;
        };

        struct Channel
        {
            uint8_t number;
            uint8_t mapping;
        };

        explicit DmaChannel(const Channel& channel);
        void ConfigureChannel(const Configuration& configuration) const;
        void ConfigureTransfer(Type type, Transfer transfer, volatile void * sourceAddress, volatile void * destinationAddress, std::size_t size) const;
        void StartTransfer() const;
        Transfer Mode(Type type) const;

    private:
        struct Control
        {
            volatile void * sourceEndAddress;
            volatile void * destinationEndAddress;
            volatile uint32_t channelControl;
            volatile uint32_t reserved;
        };

        void EnableClock() const;
        void Initialize() const;
        void DisableAllAttributes() const;
        void SetChannel() const;

    private:
        const Channel& channel;
        static std::array<uint8_t, 1024> controlTable alignas(1024);
    };
}

#endif
