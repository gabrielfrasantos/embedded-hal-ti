#ifndef HAL_GPIO_STM_HPP
#define HAL_GPIO_STM_HPP

#include DEVICE_HEADER
#include "hal/interfaces/Gpio.hpp"
#include "hal_tiva/cortex/InterruptCortex.hpp"
#include "infra/util/MemoryRange.hpp"
#include <cstdint>

namespace hal::tiva
{
    enum class Type : uint8_t
    {
        adc,
    };

    enum class Port : uint8_t
    {
        A,
        B,
        C,
        D,
        E,
        F,
        G,
        H,
        I,
        J,
        K
    };

    enum class Drive : uint8_t
    {
        Up,
        Down,
        OpenDrain,
        None,
        Default = None
    };

    enum class Current : uint8_t
    {
        _2mA,
        _4mA,
        _8mA,
        Default = _2mA
    };

    class GpioPin
        : public hal::GpioPin
    {
    public:
        GpioPin(Port port, uint8_t index, Drive drive = Drive::Default, Current current = Current::Default);

        virtual bool Get() const override;
        virtual void Set(bool value) override;
        virtual bool GetOutputLatch() const override;
        virtual void SetAsInput() override;
        virtual bool IsInput() const override;
        virtual void Config(PinConfigType config) override;
        virtual void Config(PinConfigType config, bool startOutputState) override;
        virtual void ResetConfig() override;
        virtual void EnableInterrupt(const infra::Function<void()>& action, InterruptTrigger trigger) override;
        virtual void DisableInterrupt() override;

        virtual void ConfigAnalog();
        virtual void ConfigPeripheral(uint8_t pinConfigType);

        uint32_t AdcChannel(uint8_t adc) const;

    private:
        Port port;
        uint8_t index;
        Drive drive;
        Current current;
    };

    class DummyPin
        : public GpioPin
    {
    public:
        DummyPin();

        virtual bool Get() const override;
        virtual void Set(bool value) override;
        virtual bool GetOutputLatch() const override;
        virtual void SetAsInput() override;
        virtual bool IsInput() const override;
        virtual void Config(PinConfigType config) override;
        virtual void Config(PinConfigType config, bool startOutputState) override;
        virtual void ResetConfig() override;
        virtual void EnableInterrupt(const infra::Function<void()>& action, InterruptTrigger trigger) override;
        virtual void DisableInterrupt() override;
        virtual void ConfigAnalog() override;
        virtual void ConfigPeripheral(uint8_t pinConfigType) override;
    };

    extern DummyPin dummyPin;

    class PeripheralPin
    {
    public:
        PeripheralPin(GpioPin& pin, uint8_t pinConfigType);
        PeripheralPin(const PeripheralPin& other) = delete;
        PeripheralPin& operator=(const PeripheralPin& other) = delete;
        ~PeripheralPin();

    private:
        GpioPin& pin;
    };

    class AnalogPin
    {
    public:
        AnalogPin(GpioPin& pin);
        AnalogPin(const AnalogPin& other) = delete;
        AnalogPin& operator=(const AnalogPin& other) = delete;
        ~AnalogPin();

        uint32_t AdcChannel(uint8_t adc) const;

    private:
        GpioPin& pin;
    };

    class MultiGpioPin
    {
    public:
        MultiGpioPin(infra::MemoryRange<const std::pair<Port, uint8_t>> table, Drive drive = Drive::Default, Current current = Current::Default);

        void ResetConfig();
        void ConfigAnalog();
        void ConfigPeripheral(uint8_t pinConfigType);

    private:
        infra::MemoryRange<const std::pair<Port, uint8_t>> table;
        Drive drive;
        Current current;
    };

    class MultiPeripheralPin
    {
    public:
        MultiPeripheralPin(MultiGpioPin& pins, uint8_t pinConfigType);
        MultiPeripheralPin(const MultiPeripheralPin& other) = delete;
        MultiPeripheralPin& operator=(const MultiPeripheralPin& other) = delete;
        ~MultiPeripheralPin();

    private:
        MultiGpioPin& pins;
    };

    class Gpio
        : public infra::InterfaceConnector<Gpio>
    {
    public:
        struct PinPosition
        {
            uint8_t peripheralIndex;
            Port port;
            uint8_t pin;
            uint8_t portControl;
        };

        struct AnalogPinPosition
        {
            uint8_t instance;
            Type type;
            Port port;
            uint8_t pin;
            uint8_t channel;
        };

        struct PinoutTable
        {
            uint8_t pinConfigType;
            infra::MemoryRange<const PinPosition> pinPositions;
            Drive drive;
            Current current;
        };

    public:
        Gpio(infra::MemoryRange<const infra::MemoryRange<const Gpio::PinoutTable>> pinoutTable, infra::MemoryRange<const Gpio::AnalogPinPosition> analogTable = infra::MemoryRange<const Gpio::AnalogPinPosition>());

    public:
        std::pair<const PinPosition&, const PinoutTable&> GetPeripheralPinConfig(Port port, uint8_t index, uint8_t pinConfigType) const;
        uint32_t AdcChannel(Port port, uint8_t index, uint8_t adc) const;

        void EnableInterrupt(Port port, uint8_t index, const infra::Function<void()>& action, InterruptTrigger trigger);
        void DisableInterrupt(Port port, uint8_t index);

        void ReservePin(Port port, uint8_t index);
        void ClearPinReservation(Port port, uint8_t index);

        static constexpr uint32_t registerSize = 4;

    private:
        void ExtiInterrupt(IRQn_Type irq, std::size_t from, std::size_t to);

        infra::MemoryRange<const infra::MemoryRange<const Gpio::PinoutTable>> pinoutTable;
        infra::MemoryRange<const Gpio::AnalogPinPosition> analogTable;

        std::array<infra::Function<void()>, 8 * 6> handlers;
        std::array<uint32_t, 11> assignedPins;

        DispatchedInterruptHandler interruptDispatcherA;
        DispatchedInterruptHandler interruptDispatcherB;
        DispatchedInterruptHandler interruptDispatcherC;
        DispatchedInterruptHandler interruptDispatcherD;
        DispatchedInterruptHandler interruptDispatcherE;
        DispatchedInterruptHandler interruptDispatcherF;
    };
}

#endif
