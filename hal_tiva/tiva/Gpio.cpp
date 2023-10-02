#include "hal_tiva/tiva/Gpio.hpp"
#include "infra/event/EventDispatcher.hpp"
#include "infra/util/BitLogic.hpp"
#include "infra/util/ReallyAssert.hpp"
#include <array>

#if defined(TM4C129)
#define GPIOA  GPIOA_AHB
#define GPIOB  GPIOB_AHB
#define GPIOC  GPIOC_AHB
#define GPIOD  GPIOD_AHB
#define GPIOE  GPIOE_AHB
#define GPIOF  GPIOF_AHB
#define GPIOG  GPIOG_AHB
#define GPIOH  GPIOH_AHB
#define GPIOJ  GPIOJ_AHB
#endif

namespace hal::tiva
{
    namespace
    {
        struct Peripheral
        {
            GPIOA_Type* address;
            uint32_t rcgc;
        };

        struct PushPull
        {
            bool odr;
            bool pur;
            bool pdr;
        };

        struct Mode
        {
            bool den;
            bool amsel;
            bool dir;
        };

        struct CurrentDrive
        {
            bool _2mA;
            bool _4mA;
            bool _8mA;
        };

        struct Interrupt
        {
            bool ibe;
            bool is;
            bool iev;
        };

        struct Pin
        {
            uint32_t mask;
            uint32_t bits;
        };

        // clang-format off
        const std::array<Peripheral, 15> portAndRcgc {{
            { GPIOA, 0x00000001, },
            { GPIOB, 0x00000002, },
            { GPIOC, 0x00000004, },
            { GPIOD, 0x00000008, },
            { GPIOE, 0x00000010, },
            { GPIOF, 0x00000020, },
            { GPIOG, 0x00000040, },
            { GPIOH, 0x00000080, },
            { GPIOJ, 0x00000100, },
            { GPIOK, 0x00000200, },
            { GPIOL, 0x00000400, },
            { GPIOM, 0x00000800, },
            { GPION, 0x00001000, },
            { GPIOP, 0x00002000, },
            { GPIOQ, 0x00004000, },
        }};

        const std::array<PushPull, 4> pushPullTiva{ {
            { false, true, false },     /* up */
            { false, false, true },     /* down */
            { true, false, false },     /* open drain */
            { false, false, false },    /* disable */
        } };

        const std::array<Mode, 4> modeTiva{ {
            { true, false, false },      /* in */
            { true, false, true },     /* out */
            { false, false, false },    /* alternate */
            { false, true, true },      /* analog */
        } };

        const std::array<CurrentDrive, 4> currentDriveTiva{ {
            { true, false, false },    /* 2mA */
            { false, true, false },    /* 4mA */
            { false, false, true },    /* 8mA */
            { true, false, false },    /* 2mA */
        } };

        const std::array<Interrupt, 4> interruptTiva{ {
            { false, false, true },     /* rising */
            { false, false, false },    /* falling */
            { true, false, false },     /* both */
            { false, false, false },    /* none */
        } };

        const std::array<Pin, 8> pinTiva {{
            { 0xFFFFFFF0, 0 },
            { 0xFFFFFF0F, 4 },
            { 0xFFFFF0FF, 8 },
            { 0xFFFF0FFF, 12 },
            { 0xFFF0FFFF, 16 },
            { 0xFF0FFFFF, 20 },
            { 0xF0FFFFFF, 24 },
            { 0x0FFFFFFF, 28 },
        }};
        // clang-format on

        constexpr GPIOA_Type* GpioTiva(Port port)
        {
            return portAndRcgc[static_cast<uint8_t>(port)].address;
        }

        constexpr uint32_t Rcgc(Port port)
        {
            return portAndRcgc[static_cast<uint8_t>(port)].rcgc;
        }

        constexpr uint32_t ToPin(uint8_t index)
        {
            return 1 << index;
        }

        uint8_t ToPctl(const infra::MemoryRange<const Gpio::PinPosition>& pins, Port port, uint8_t index)
        {
            for (auto& pin : pins)
                if (pin.pin == index && pin.port == port)
                    return pin.portControl;

            std::abort();
        }

        template<class T>
        constexpr bool GetBit(T& reg, uint32_t position)
        {
            return reg & (1 << position);
        }
    }

    DummyPin dummyPin;

    GpioPin::GpioPin(Port port, uint8_t index, Drive drive, Current current)
        : port(port)
        , index(index)
        , drive(drive)
        , current(current)
    {
        SYSCTL->RCGCGPIO = SYSCTL->RCGCGPIO | Rcgc(port);

        while (!(SYSCTL->PRGPIO & Rcgc(port)))
        {
        }

        if (((GpioTiva(port) == GPIOF) && (index == 0)) ||
            ((GpioTiva(port) == GPIOD) && (index == 7)))
        {
            GpioTiva(port)->LOCK = 0x4C4F434B;
            infra::ReplaceBit(GpioTiva(port)->CR, true, index);
            GpioTiva(port)->LOCK = 0;
        }
    }

    bool GpioPin::Get() const
    {
        return infra::IsBitSet(GpioTiva(port)->DATA, index);
    }

    void GpioPin::Set(bool value)
    {
        infra::ReplaceBit(GpioTiva(port)->DATA, value, index);
    }

    bool GpioPin::GetOutputLatch() const
    {
        return infra::IsBitSet(GpioTiva(port)->DATA, index);
    }

    void GpioPin::SetAsInput()
    {
        infra::ReplaceBit(GpioTiva(port)->DIR, true, index);
    }

    bool GpioPin::IsInput() const
    {
        return GetBit(GpioTiva(port)->DIR, index) == 0;
    }

    void GpioPin::Config(PinConfigType config)
    {
        Gpio::Instance().ReservePin(port, index);

        auto gpio = GpioTiva(port);

        infra::ReplaceBit(GpioTiva(port)->DIR, modeTiva[static_cast<uint8_t>(config)].dir, index);
        infra::ReplaceBit(GpioTiva(port)->AFSEL, false, index);
        infra::ReplaceBit(GpioTiva(port)->DR2R, currentDriveTiva[static_cast<uint8_t>(current)]._2mA, index);
        infra::ReplaceBit(GpioTiva(port)->DR4R, currentDriveTiva[static_cast<uint8_t>(current)]._4mA, index);
        infra::ReplaceBit(GpioTiva(port)->DR8R, currentDriveTiva[static_cast<uint8_t>(current)]._8mA, index);

        infra::ReplaceBit(GpioTiva(port)->DEN, modeTiva[static_cast<uint8_t>(config)].den, index);
        infra::ReplaceBit(GpioTiva(port)->AMSEL, modeTiva[static_cast<uint8_t>(config)].amsel, index);

        infra::ReplaceBit(GpioTiva(port)->PUR, pushPullTiva[static_cast<uint8_t>(drive)].pur, index);
        infra::ReplaceBit(GpioTiva(port)->PDR, pushPullTiva[static_cast<uint8_t>(drive)].pdr, index);
        infra::ReplaceBit(GpioTiva(port)->ODR, pushPullTiva[static_cast<uint8_t>(drive)].odr, index);

        GpioTiva(port)->PCTL = (GpioTiva(port)->PCTL & pinTiva[index].mask) | 0 << pinTiva[index].bits;
    }

    void GpioPin::Config(PinConfigType config, bool startOutputState)
    {
        Config(config);

        if (config != PinConfigType::input)
            Set(startOutputState);
    }

    void GpioPin::ResetConfig()
    {
        infra::ReplaceBit(GpioTiva(port)->DIR, true, index);
        infra::ReplaceBit(GpioTiva(port)->DEN, true, index);
        infra::ReplaceBit(GpioTiva(port)->AMSEL, false, index);

        infra::ReplaceBit(GpioTiva(port)->PUR, false, index);
        infra::ReplaceBit(GpioTiva(port)->PDR, false, index);
        infra::ReplaceBit(GpioTiva(port)->ODR, false, index);

        infra::ReplaceBit(GpioTiva(port)->AFSEL, false, index);
        GpioTiva(port)->PCTL = (GpioTiva(port)->PCTL & pinTiva[index].mask) | 0 << pinTiva[index].bits;

        infra::ReplaceBit(GpioTiva(port)->DR2R, true, index);
        infra::ReplaceBit(GpioTiva(port)->DR4R, false, index);
        infra::ReplaceBit(GpioTiva(port)->DR8R, false, index);

        Gpio::Instance().ClearPinReservation(port, index);
    }

    void GpioPin::EnableInterrupt(const infra::Function<void()>& action, InterruptTrigger trigger)
    {
        Gpio::Instance().EnableInterrupt(port, index, action, trigger);
    }

    void GpioPin::DisableInterrupt()
    {
        Gpio::Instance().DisableInterrupt(port, index);
    }

    void GpioPin::ConfigAnalog()
    {
        Gpio::Instance().ReservePin(port, index);

        infra::ReplaceBit(GpioTiva(port)->DIR, true, index);
        infra::ReplaceBit(GpioTiva(port)->DEN, false, index);
        infra::ReplaceBit(GpioTiva(port)->AMSEL, true, index);
    }

    void GpioPin::ConfigPeripheral(PinConfigPeripheral pinConfigType)
    {
        std::pair<const Gpio::PinPosition&, const Gpio::PinoutTable&> peripheralPinConfig = Gpio::Instance().GetPeripheralPinConfig(port, index, pinConfigType);

        Gpio::Instance().ReservePin(port, index);

        auto portControl = peripheralPinConfig.second;
        auto drive = peripheralPinConfig.second.drive;
        auto current = peripheralPinConfig.second.current;

        infra::ReplaceBit(GpioTiva(port)->DIR, modeTiva[static_cast<uint8_t>(portControl.config)].dir, index);
        infra::ReplaceBit(GpioTiva(port)->DEN, portControl.isDigital, index);
        infra::ReplaceBit(GpioTiva(port)->AMSEL, !portControl.isDigital, index);

        GpioTiva(port)->PCTL = (GpioTiva(port)->PCTL & pinTiva[index].mask) | ToPctl(portControl.pinPositions, port, index) << pinTiva[index].bits;

        infra::ReplaceBit(GpioTiva(port)->AFSEL, true, index);
        infra::ReplaceBit(GpioTiva(port)->DR2R, currentDriveTiva[static_cast<uint8_t>(current)]._2mA, index);
        infra::ReplaceBit(GpioTiva(port)->DR4R, currentDriveTiva[static_cast<uint8_t>(current)]._4mA, index);
        infra::ReplaceBit(GpioTiva(port)->DR8R, currentDriveTiva[static_cast<uint8_t>(current)]._8mA, index);

        infra::ReplaceBit(GpioTiva(port)->PUR, pushPullTiva[static_cast<uint8_t>(drive)].pur, index);
        infra::ReplaceBit(GpioTiva(port)->PDR, pushPullTiva[static_cast<uint8_t>(drive)].pdr, index);
        infra::ReplaceBit(GpioTiva(port)->ODR, pushPullTiva[static_cast<uint8_t>(drive)].odr, index);
    }

    uint32_t GpioPin::AdcChannel() const
    {
        return Gpio::Instance().AdcChannel(port, index);
    }

    DummyPin::DummyPin()
        : GpioPin(Port::A, 0)
    {}

    bool DummyPin::Get() const
    {
        return false;
    }

    void DummyPin::Set(bool value)
    {}

    bool DummyPin::GetOutputLatch() const
    {
        return false;
    }

    void DummyPin::SetAsInput()
    {}

    bool DummyPin::IsInput() const
    {
        return false;
    }

    void DummyPin::Config(PinConfigType config)
    {}

    void DummyPin::Config(PinConfigType config, bool startOutputState)
    {}

    void DummyPin::ResetConfig()
    {}

    void DummyPin::EnableInterrupt(const infra::Function<void()>& action, InterruptTrigger trigger)
    {}

    void DummyPin::DisableInterrupt()
    {}

    void DummyPin::ConfigAnalog()
    {}

    void DummyPin::ConfigPeripheral(PinConfigPeripheral pinConfigType)
    {}

    PeripheralPin::PeripheralPin(GpioPin& pin, PinConfigPeripheral pinConfigType)
        : pin(pin)
    {
        pin.ConfigPeripheral(pinConfigType);
    }

    PeripheralPin::~PeripheralPin()
    {
        pin.ResetConfig();
    }

    AnalogPin::AnalogPin(GpioPin& pin)
        : pin(pin)
    {
        pin.ConfigAnalog();
    }

    AnalogPin::~AnalogPin()
    {
        pin.ResetConfig();
    }

    uint32_t AnalogPin::AdcChannel() const
    {
        return pin.AdcChannel();
    }

    MultiGpioPin::MultiGpioPin(infra::MemoryRange<const std::pair<Port, uint8_t>> table, Drive drive, Current current)
        : table(table)
        , drive(drive)
        , current(current)
    {
        for (const std::pair<Port, uint8_t>& portAndIndex : table)
        {
            SYSCTL->RCGCGPIO = SYSCTL->RCGCGPIO | Rcgc(portAndIndex.first); // NOLINT

            while (!(SYSCTL->PRGPIO & Rcgc(portAndIndex.first))) //NOLINT
            {
            }

            if (((GpioTiva(portAndIndex.first) == GPIOF) && (portAndIndex.second == 0)) ||
                ((GpioTiva(portAndIndex.first) == GPIOD) && (portAndIndex.second == 7))) // NOLINT
            {
                GpioTiva(portAndIndex.first)->LOCK = 0x4C4F434B;
                infra::ReplaceBit(GpioTiva(portAndIndex.first)->CR, true, portAndIndex.second);
                GpioTiva(portAndIndex.first)->LOCK = 0;
            }
        }
    }

    void MultiGpioPin::ResetConfig()
    {
        for (const std::pair<Port, uint8_t>& portAndIndex : table)
        {
            infra::ReplaceBit(GpioTiva(portAndIndex.first)->DIR, true, portAndIndex.second);
            infra::ReplaceBit(GpioTiva(portAndIndex.first)->DEN, true, portAndIndex.second);
            infra::ReplaceBit(GpioTiva(portAndIndex.first)->AMSEL, false, portAndIndex.second);

            infra::ReplaceBit(GpioTiva(portAndIndex.first)->PUR, false, portAndIndex.second);
            infra::ReplaceBit(GpioTiva(portAndIndex.first)->PDR, false, portAndIndex.second);
            infra::ReplaceBit(GpioTiva(portAndIndex.first)->ODR, false, portAndIndex.second);

            infra::ReplaceBit(GpioTiva(portAndIndex.first)->AFSEL, false, portAndIndex.second);
            GpioTiva(portAndIndex.first)->PCTL = (GpioTiva(portAndIndex.first)->PCTL & pinTiva[portAndIndex.second].mask) | 0 << pinTiva[portAndIndex.second].bits;

            infra::ReplaceBit(GpioTiva(portAndIndex.first)->DR2R, true, portAndIndex.second);
            infra::ReplaceBit(GpioTiva(portAndIndex.first)->DR4R, false, portAndIndex.second);
            infra::ReplaceBit(GpioTiva(portAndIndex.first)->DR8R, false, portAndIndex.second);

            Gpio::Instance().ClearPinReservation(portAndIndex.first, portAndIndex.second);
        }
    }

    void MultiGpioPin::ConfigAnalog()
    {
        for (const std::pair<Port, uint8_t>& portAndIndex : table)
        {
            Gpio::Instance().ReservePin(portAndIndex.first, portAndIndex.second);

            infra::ReplaceBit(GpioTiva(portAndIndex.first)->DIR, true, portAndIndex.second);
            infra::ReplaceBit(GpioTiva(portAndIndex.first)->DEN, false, portAndIndex.second);
            infra::ReplaceBit(GpioTiva(portAndIndex.first)->AMSEL, true, portAndIndex.second);
        }
    }

    void MultiGpioPin::ConfigPeripheral(PinConfigPeripheral pinConfigType)
    {
        for (const std::pair<Port, uint8_t>& portAndIndex : table)
        {
            std::pair<const Gpio::PinPosition&, const Gpio::PinoutTable&> peripheralPinConfig = Gpio::Instance().GetPeripheralPinConfig(portAndIndex.first, portAndIndex.second, pinConfigType);

            Gpio::Instance().ReservePin(portAndIndex.first, portAndIndex.second);

            auto portControl = peripheralPinConfig.second;

            infra::ReplaceBit(GpioTiva(portAndIndex.first)->DIR, modeTiva[static_cast<uint8_t>(portControl.config)].dir, portAndIndex.second);
            infra::ReplaceBit(GpioTiva(portAndIndex.first)->DEN, portControl.isDigital, portAndIndex.second);
            infra::ReplaceBit(GpioTiva(portAndIndex.first)->AMSEL, !portControl.isDigital, portAndIndex.second);

            GpioTiva(portAndIndex.first)->PCTL = (GpioTiva(portAndIndex.first)->PCTL & pinTiva[portAndIndex.second].mask) | 0 << pinTiva[portAndIndex.second].bits;

            infra::ReplaceBit(GpioTiva(portAndIndex.first)->PUR, pushPullTiva[static_cast<uint8_t>(drive)].pur, portAndIndex.second);
            infra::ReplaceBit(GpioTiva(portAndIndex.first)->PDR, pushPullTiva[static_cast<uint8_t>(drive)].pdr, portAndIndex.second);
            infra::ReplaceBit(GpioTiva(portAndIndex.first)->ODR, pushPullTiva[static_cast<uint8_t>(drive)].odr, portAndIndex.second);

            infra::ReplaceBit(GpioTiva(portAndIndex.first)->AFSEL, true, portAndIndex.second);
            GpioTiva(portAndIndex.first)->PCTL = (GpioTiva(portAndIndex.first)->PCTL & pinTiva[portAndIndex.second].mask) | ToPctl(portControl.pinPositions, portAndIndex.first, portAndIndex.second) << pinTiva[portAndIndex.second].bits;

            infra::ReplaceBit(GpioTiva(portAndIndex.first)->DR2R, currentDriveTiva[static_cast<uint8_t>(current)]._2mA, portAndIndex.second);
            infra::ReplaceBit(GpioTiva(portAndIndex.first)->DR4R, currentDriveTiva[static_cast<uint8_t>(current)]._4mA, portAndIndex.second);
            infra::ReplaceBit(GpioTiva(portAndIndex.first)->DR8R, currentDriveTiva[static_cast<uint8_t>(current)]._8mA, portAndIndex.second);
        }
    }

    MultiPeripheralPin::MultiPeripheralPin(MultiGpioPin& pins, PinConfigPeripheral pinConfigType)
        : pins(pins)
    {
        pins.ConfigPeripheral(pinConfigType);
    }

    MultiPeripheralPin::~MultiPeripheralPin()
    {
        pins.ResetConfig();
    }

    // clang-format off
    Gpio::Gpio(infra::MemoryRange<const infra::MemoryRange<const Gpio::PinoutTable>> pinoutTable, infra::MemoryRange<const Gpio::AnalogPinPosition> analogTable)
        : pinoutTable(pinoutTable)
        , analogTable(analogTable)
        , assignedPins()
        , interruptDispatcherA(GPIOA_IRQn, [this]()
            { ExtiInterrupt(GPIOA, 0, 8); })
        , interruptDispatcherB(GPIOB_IRQn, [this]()
            { ExtiInterrupt(GPIOB, 0, 8); })
        , interruptDispatcherC(GPIOC_IRQn, [this]()
            { ExtiInterrupt(GPIOC, 0, 8); })
        , interruptDispatcherD(GPIOD_IRQn, [this]()
            { ExtiInterrupt(GPIOD, 0, 8); })
        , interruptDispatcherE(GPIOE_IRQn, [this]()
            { ExtiInterrupt(GPIOE, 0, 8); })
        , interruptDispatcherF(GPIOF_IRQn, [this]()
            { ExtiInterrupt(GPIOF, 0, 8); })
    { }
    // clang-format on

    std::pair<const Gpio::PinPosition&, const Gpio::PinoutTable&> Gpio::GetPeripheralPinConfig(Port port, uint8_t index, PinConfigPeripheral pinConfigType) const
    {
        for (infra::MemoryRange<const Gpio::PinoutTable> subTable : pinoutTable)
            for (const PinoutTable& table : subTable)
                if (table.pinConfigType == pinConfigType)
                    for (const PinPosition& position : table.pinPositions)
                        if (position.port == port && position.pin == index)
                            return std::pair<const Gpio::PinPosition&, const Gpio::PinoutTable&>(position, table);

        abort();
    }

    uint32_t Gpio::AdcChannel(Port port, uint8_t pin) const
    {
        for (const Gpio::AnalogPinPosition position : analogTable)
            if (position.type == Type::adc && position.port == port && position.pin == pin)
                return position.channel;

        abort();
    }

    void Gpio::EnableInterrupt(Port port, uint8_t index, const infra::Function<void()>& action, InterruptTrigger trigger)
    {
        infra::ReplaceBit(GpioTiva(port)->IM, false, index);

        infra::ReplaceBit(GpioTiva(port)->IBE, interruptTiva[static_cast<uint8_t>(trigger)].ibe, index);
        infra::ReplaceBit(GpioTiva(port)->IS, interruptTiva[static_cast<uint8_t>(trigger)].is, index);
        infra::ReplaceBit(GpioTiva(port)->IEV, interruptTiva[static_cast<uint8_t>(trigger)].iev, index);

        really_assert(!handlers[index]);
        handlers[index] = action;

        infra::ReplaceBit(GpioTiva(port)->RIS, false, index);
        infra::ReplaceBit(GpioTiva(port)->ICR, true, index);
        infra::ReplaceBit(GpioTiva(port)->IM, true, index);
    }

    void Gpio::DisableInterrupt(Port port, uint8_t index)
    {
        infra::ReplaceBit(GpioTiva(port)->IM, false, index);

        handlers[index] = nullptr;
    }

    void Gpio::ExtiInterrupt(GPIOA_Type* gpio, std::size_t from, std::size_t to)
    {
        for (std::size_t line = from; line != to; ++line)
        {
            if (infra::IsBitSet(gpio->RIS, line))
            {
                infra::ReplaceBit(gpio->ICR, true, line);

                if (handlers[line])
                    infra::EventDispatcher::Instance().Schedule(handlers[line]);
            }
        }
    }

    void Gpio::ReservePin(Port port, uint8_t index)
    {
        assert(static_cast<uint8_t>(port) < assignedPins.size());
        assert(index < 8 && index >= 0);
        assert((assignedPins[static_cast<uint8_t>(port)] & (1 << index)) == 0);
        assignedPins[static_cast<uint8_t>(port)] |= 1 << index;
    }

    void Gpio::ClearPinReservation(Port port, uint8_t index)
    {
        assert(static_cast<uint8_t>(port) < assignedPins.size());
        assert(index < 8 && index >= 0);
        assignedPins[static_cast<uint8_t>(port)] &= ~(1 << index);
    }
}
