#include "hal_tiva/tiva/Gpio.hpp"
#include "infra/event/EventDispatcher.hpp"
#include "infra/util/BitLogic.hpp"
#include "infra/util/ReallyAssert.hpp"
#include <array>

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
        const std::array<Peripheral, 8> portAndRcgc {{
            { GPIOA, 0x01, }, // NOLINT
            { GPIOB, 0x02, }, // NOLINT
            { GPIOC, 0x04, }, // NOLINT
            { GPIOD, 0x08, }, // NOLINT
            { GPIOE, 0x10, }, // NOLINT
            { GPIOF, 0x20, }, // NOLINT
        }};

        const std::array<PushPull, 4> pushPullTiva{ {
            { false, true, false },     /* UP */
            { false, false, true },     /* DOWN */
            { true, false, false },     /* Open drain */
            { false, false, false },    /* Disable */
        } };

        const std::array<Mode, 4> modeTiva{ {
            { true, false, true },      /* in */
            { true, false, false },     /* out */
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
            { false, false, true },     /* RISING */
            { false, false, false },    /* FALLING */
            { true, false, false },     /* BOTH */
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
    }

    DummyPin dummyPin;

    GpioPin::GpioPin(Port port, uint8_t index, Drive drive, Current current)
        : port(port)
        , index(index)
        , drive(drive)
        , current(current)
    { 
        SYSCTL->RCGCGPIO = SYSCTL->RCGCGPIO | Rcgc(port); // NOLINT

        while (!(SYSCTL->PRGPIO & Rcgc(port))) //NOLINT
        {
        }

        if (((GpioTiva(port) == GPIOF) && (index == 0)) || 
            ((GpioTiva(port) == GPIOD) && (index == 7))) // NOLINT
        {
            GpioTiva(port)->LOCK = 0x4C4F434B;
            infra::ReplaceBits(GpioTiva(port)->CR, Gpio::registerSize, 1, index);
            GpioTiva(port)->LOCK = 0;
        }
    }

    bool GpioPin::Get() const
    {
        return infra::IsBitSet(GpioTiva(port)->DATA, index);
    }

    void GpioPin::Set(bool value)
    {
        infra::ReplaceBits(GpioTiva(port)->DIR, Gpio::registerSize, value, index);
    }

    bool GpioPin::GetOutputLatch() const
    {
        return infra::IsBitSet(GpioTiva(port)->DATA, index);
    }

    void GpioPin::SetAsInput()
    {
        infra::ReplaceBits(GpioTiva(port)->DIR, Gpio::registerSize, 1, index);
    }

    bool GpioPin::IsInput() const
    {
        return infra::GetBits(GpioTiva(port)->DIR, Gpio::registerSize, index) == 0;
    }

    void GpioPin::Config(PinConfigType config)
    {
        assert(config != PinConfigType::output); // Use Config(_, startOutputState) for outputs

        Gpio::Instance().ReservePin(port, index);

        infra::ReplaceBits(GpioTiva(port)->DIR, Gpio::registerSize, modeTiva[static_cast<uint8_t>(config)].dir, index);
        infra::ReplaceBits(GpioTiva(port)->DEN, Gpio::registerSize, modeTiva[static_cast<uint8_t>(config)].den, index);
        infra::ReplaceBits(GpioTiva(port)->AMSEL, Gpio::registerSize, modeTiva[static_cast<uint8_t>(config)].amsel, index);

        infra::ReplaceBits(GpioTiva(port)->PUR, Gpio::registerSize, static_cast<uint8_t>(drive), index);
        infra::ReplaceBits(GpioTiva(port)->PDR, Gpio::registerSize, static_cast<uint8_t>(drive), index);
        infra::ReplaceBits(GpioTiva(port)->ODR, Gpio::registerSize, static_cast<uint8_t>(drive), index);

        infra::ReplaceBits(GpioTiva(port)->AFSEL, Gpio::registerSize, 0, index);
        infra::MaskedUpdate(GpioTiva(port)->PCTL, ~pinTiva[index].mask, 0);

        infra::ReplaceBits(GpioTiva(port)->DR2R, Gpio::registerSize, currentDriveTiva[static_cast<uint8_t>(current)]._2mA, index);
        infra::ReplaceBits(GpioTiva(port)->DR4R, Gpio::registerSize, currentDriveTiva[static_cast<uint8_t>(current)]._4mA, index);
        infra::ReplaceBits(GpioTiva(port)->DR8R, Gpio::registerSize, currentDriveTiva[static_cast<uint8_t>(current)]._8mA, index);
    }

    void GpioPin::Config(PinConfigType config, bool startOutputState)
    {
        assert(config != PinConfigType::input);

        Gpio::Instance().ReservePin(port, index);

        infra::ReplaceBits(GpioTiva(port)->DIR, Gpio::registerSize, modeTiva[static_cast<uint8_t>(config)].dir, index);
        infra::ReplaceBits(GpioTiva(port)->DEN, Gpio::registerSize, modeTiva[static_cast<uint8_t>(config)].den, index);
        infra::ReplaceBits(GpioTiva(port)->AMSEL, Gpio::registerSize, modeTiva[static_cast<uint8_t>(config)].amsel, index);

        infra::ReplaceBits(GpioTiva(port)->PUR, Gpio::registerSize, static_cast<uint8_t>(drive), index);
        infra::ReplaceBits(GpioTiva(port)->PDR, Gpio::registerSize, static_cast<uint8_t>(drive), index);
        infra::ReplaceBits(GpioTiva(port)->ODR, Gpio::registerSize, static_cast<uint8_t>(drive), index);

        infra::ReplaceBits(GpioTiva(port)->AFSEL, Gpio::registerSize, 0, index);
        infra::MaskedUpdate(GpioTiva(port)->PCTL, ~pinTiva[index].mask, 0);

        infra::ReplaceBits(GpioTiva(port)->DR2R, Gpio::registerSize, currentDriveTiva[static_cast<uint8_t>(current)]._2mA, index);
        infra::ReplaceBits(GpioTiva(port)->DR4R, Gpio::registerSize, currentDriveTiva[static_cast<uint8_t>(current)]._4mA, index);
        infra::ReplaceBits(GpioTiva(port)->DR8R, Gpio::registerSize, currentDriveTiva[static_cast<uint8_t>(current)]._8mA, index);

        Set(startOutputState);
    }

    void GpioPin::ResetConfig()
    {
        infra::ReplaceBits(GpioTiva(port)->DIR, Gpio::registerSize, true, index);
        infra::ReplaceBits(GpioTiva(port)->DEN, Gpio::registerSize, true, index);
        infra::ReplaceBits(GpioTiva(port)->AMSEL, Gpio::registerSize, false, index);

        infra::ReplaceBits(GpioTiva(port)->PUR, Gpio::registerSize, false, index);
        infra::ReplaceBits(GpioTiva(port)->PDR, Gpio::registerSize, false, index);
        infra::ReplaceBits(GpioTiva(port)->ODR, Gpio::registerSize, false, index);

        infra::ReplaceBits(GpioTiva(port)->AFSEL, Gpio::registerSize, 0, index);
        infra::MaskedUpdate(GpioTiva(port)->PCTL, ~pinTiva[index].mask, 0);

        infra::ReplaceBits(GpioTiva(port)->DR2R, Gpio::registerSize, true, index);
        infra::ReplaceBits(GpioTiva(port)->DR4R, Gpio::registerSize, false, index);
        infra::ReplaceBits(GpioTiva(port)->DR8R, Gpio::registerSize, false, index);

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

        infra::ReplaceBits(GpioTiva(port)->DIR, Gpio::registerSize, true, index);
        infra::ReplaceBits(GpioTiva(port)->DEN, Gpio::registerSize, false, index);
        infra::ReplaceBits(GpioTiva(port)->AMSEL, Gpio::registerSize, true, index);
    }

    void GpioPin::ConfigPeripheral(PinConfigPeripheral pinConfigType)
    {
        std::pair<const Gpio::PinPosition&, const Gpio::PinoutTable&> peripheralPinConfig = Gpio::Instance().GetPeripheralPinConfig(port, index, pinConfigType);

        Gpio::Instance().ReservePin(port, index);

        uint8_t portControl = peripheralPinConfig.first.portControl;
        Drive drive = peripheralPinConfig.second.drive;
        Current current = peripheralPinConfig.second.current;

        // Peripherals use the weakpull, drive, and current from the peripheral's definition
#if 0
        infra::ReplaceBits(GpioTiva(port)->DIR, Gpio::registerSize, modeTiva[static_cast<uint8_t>(config)].dir, index);
        infra::ReplaceBits(GpioTiva(port)->DEN, Gpio::registerSize, modeTiva[static_cast<uint8_t>(config)].den, index);
        infra::ReplaceBits(GpioTiva(port)->AMSEL, Gpio::registerSize, modeTiva[static_cast<uint8_t>(config)].amsel, index);
#endif

        infra::ReplaceBits(GpioTiva(port)->PUR, Gpio::registerSize, static_cast<uint8_t>(drive), index);
        infra::ReplaceBits(GpioTiva(port)->PDR, Gpio::registerSize, static_cast<uint8_t>(drive), index);
        infra::ReplaceBits(GpioTiva(port)->ODR, Gpio::registerSize, static_cast<uint8_t>(drive), index);

        infra::ReplaceBits(GpioTiva(port)->AFSEL, Gpio::registerSize, true, index);
        infra::MaskedUpdate(GpioTiva(port)->PCTL, pinTiva[index].mask, portControl);

        infra::ReplaceBits(GpioTiva(port)->DR2R, Gpio::registerSize, currentDriveTiva[static_cast<uint8_t>(current)]._2mA, index);
        infra::ReplaceBits(GpioTiva(port)->DR4R, Gpio::registerSize, currentDriveTiva[static_cast<uint8_t>(current)]._4mA, index);
        infra::ReplaceBits(GpioTiva(port)->DR8R, Gpio::registerSize, currentDriveTiva[static_cast<uint8_t>(current)]._8mA, index);
    }

    uint32_t GpioPin::AdcChannel(uint8_t adc) const
    {
        return Gpio::Instance().AdcChannel(port, index, adc);
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

    uint32_t AnalogPin::AdcChannel(uint8_t adc) const
    {
        return pin.AdcChannel(adc);
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
                infra::ReplaceBits(GpioTiva(portAndIndex.first)->CR, Gpio::registerSize, true, portAndIndex.second);
                GpioTiva(portAndIndex.first)->LOCK = 0;
            }
        }
    }

    void MultiGpioPin::ResetConfig()
    {
        for (const std::pair<Port, uint8_t>& portAndIndex : table)
        {
            infra::ReplaceBits(GpioTiva(portAndIndex.first)->DIR, Gpio::registerSize, true, portAndIndex.second);
            infra::ReplaceBits(GpioTiva(portAndIndex.first)->DEN, Gpio::registerSize, true, portAndIndex.second);
            infra::ReplaceBits(GpioTiva(portAndIndex.first)->AMSEL, Gpio::registerSize, false, portAndIndex.second);

            infra::ReplaceBits(GpioTiva(portAndIndex.first)->PUR, Gpio::registerSize, false, portAndIndex.second);
            infra::ReplaceBits(GpioTiva(portAndIndex.first)->PDR, Gpio::registerSize, false, portAndIndex.second);
            infra::ReplaceBits(GpioTiva(portAndIndex.first)->ODR, Gpio::registerSize, false, portAndIndex.second);

            infra::ReplaceBits(GpioTiva(portAndIndex.first)->AFSEL, Gpio::registerSize, 0, portAndIndex.second);
            infra::MaskedUpdate(GpioTiva(portAndIndex.first)->PCTL, ~pinTiva[portAndIndex.second].mask, 0);

            infra::ReplaceBits(GpioTiva(portAndIndex.first)->DR2R, Gpio::registerSize, true, portAndIndex.second);
            infra::ReplaceBits(GpioTiva(portAndIndex.first)->DR4R, Gpio::registerSize, false, portAndIndex.second);
            infra::ReplaceBits(GpioTiva(portAndIndex.first)->DR8R, Gpio::registerSize, false, portAndIndex.second);

            Gpio::Instance().ClearPinReservation(portAndIndex.first, portAndIndex.second);
        }
    }

    void MultiGpioPin::ConfigAnalog()
    {
        for (const std::pair<Port, uint8_t>& portAndIndex : table)
        {
            Gpio::Instance().ReservePin(portAndIndex.first, portAndIndex.second);

            infra::ReplaceBits(GpioTiva(portAndIndex.first)->DIR, Gpio::registerSize, true, portAndIndex.second);
            infra::ReplaceBits(GpioTiva(portAndIndex.first)->DEN, Gpio::registerSize, false, portAndIndex.second);
            infra::ReplaceBits(GpioTiva(portAndIndex.first)->AMSEL, Gpio::registerSize, true, portAndIndex.second);
        }
    }

    void MultiGpioPin::ConfigPeripheral(PinConfigPeripheral pinConfigType)
    {
        for (const std::pair<Port, uint8_t>& portAndIndex : table)
        {
            std::pair<const Gpio::PinPosition&, const Gpio::PinoutTable&> peripheralPinConfig = Gpio::Instance().GetPeripheralPinConfig(portAndIndex.first, portAndIndex.second, pinConfigType);

            Gpio::Instance().ReservePin(portAndIndex.first, portAndIndex.second);

#if 0
            infra::ReplaceBits(GpioTiva(portAndIndex.first)->DIR, Gpio::registerSize, modeTiva[static_cast<uint8_t>(config)].dir, portAndIndex.second);
            infra::ReplaceBits(GpioTiva(portAndIndex.first)->DEN, Gpio::registerSize, modeTiva[static_cast<uint8_t>(config)].den, portAndIndex.second);
            infra::ReplaceBits(GpioTiva(portAndIndex.first)->AMSEL, Gpio::registerSize, modeTiva[static_cast<uint8_t>(config)].amsel, portAndIndex.second);
#endif

            infra::ReplaceBits(GpioTiva(portAndIndex.first)->PUR, Gpio::registerSize, static_cast<uint8_t>(drive), portAndIndex.second);
            infra::ReplaceBits(GpioTiva(portAndIndex.first)->PDR, Gpio::registerSize, static_cast<uint8_t>(drive), portAndIndex.second);
            infra::ReplaceBits(GpioTiva(portAndIndex.first)->ODR, Gpio::registerSize, static_cast<uint8_t>(drive), portAndIndex.second);

            infra::ReplaceBits(GpioTiva(portAndIndex.first)->AFSEL, Gpio::registerSize, true, portAndIndex.second);
            infra::MaskedUpdate(GpioTiva(portAndIndex.first)->PCTL, pinTiva[portAndIndex.second].mask, peripheralPinConfig.first.portControl);

            infra::ReplaceBits(GpioTiva(portAndIndex.first)->DR2R, Gpio::registerSize, currentDriveTiva[static_cast<uint8_t>(current)]._2mA, portAndIndex.second);
            infra::ReplaceBits(GpioTiva(portAndIndex.first)->DR4R, Gpio::registerSize, currentDriveTiva[static_cast<uint8_t>(current)]._4mA, portAndIndex.second);
            infra::ReplaceBits(GpioTiva(portAndIndex.first)->DR8R, Gpio::registerSize, currentDriveTiva[static_cast<uint8_t>(current)]._8mA, portAndIndex.second);
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

    uint32_t Gpio::AdcChannel(Port port, uint8_t pin, uint8_t instance) const
    {
        for (const Gpio::AnalogPinPosition position : analogTable)
            if (position.type == Type::adc && position.instance == instance && position.port == port && position.pin == pin)
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
