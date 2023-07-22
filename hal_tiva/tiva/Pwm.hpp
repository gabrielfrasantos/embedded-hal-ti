#ifndef HAL_PWM_TIVA_HPP
#define HAL_PWM_TIVA_HPP

//#include "hal/interfaces/Spi.hpp"
#include "hal_tiva/cortex/InterruptCortex.hpp"
#include "hal_tiva/tiva/Gpio.hpp"
#include "infra/util/BoundedVector.hpp"
#include "infra/util/Function.hpp"
#include "infra/util/Optional.hpp"

namespace hal::tiva
{
    class Pwm
        //: public hal::Pwm
    {
    public:
        struct Config
        {
            enum class UnitClockDivisor
            {
                _1,
                _2,
                _4,
                _8,
                _16,
                _32,
                _64,
            };

            struct Control
            {
                enum class Mode
                {
                    edgeAligned = 0,
                    centerAligned,
                };

                enum class UpdateMode
                {
                    immediate = 0,
                    locally = 2,
                    globally = 3,
                };

                Mode mode = Mode::edgeAligned;
                UpdateMode updateMode = UpdateMode::immediate;

                uint32_t Value() const
                {
                    uint32_t value = static_cast<uint32_t>(mode) << 1;

                    value |= static_cast<uint32_t>(updateMode == UpdateMode::globally) << 3;
                    value |= static_cast<uint32_t>(updateMode == UpdateMode::globally) << 4;
                    value |= static_cast<uint32_t>(updateMode == UpdateMode::globally) << 5;

                    value |= static_cast<uint32_t>(updateMode) << 6;
                    value |= static_cast<uint32_t>(updateMode) << 8;
                    value |= static_cast<uint32_t>(updateMode) << 10;
                    value |= static_cast<uint32_t>(updateMode) << 12;
                    value |= static_cast<uint32_t>(updateMode) << 14;

                    return value & 0xfffe;
                }
            };

            struct Generator
            {
                enum class Action
                {
                    doNothing = 0,
                    invert = 1,
                    low = 2,
                    high = 3,
                };

                Action comparatorBDown = Action::doNothing;
                Action comparatorBUp = Action::doNothing;
                Action comparatorADown = Action::low;
                Action comparatorAUp = Action::doNothing;
                Action counterLoad = Action::high;
                Action counterZero = Action::doNothing;

                uint32_t Value() const
                {
                    uint32_t value = static_cast<uint32_t>(counterZero);

                    value |= static_cast<uint32_t>(counterLoad) << 2;
                    value |= static_cast<uint32_t>(comparatorAUp) << 4;
                    value |= static_cast<uint32_t>(comparatorADown) << 6;
                    value |= static_cast<uint32_t>(comparatorBUp) << 8;
                    value |= static_cast<uint32_t>(comparatorBDown) << 10;

                    return value;
                }
            };

            constexpr Config()
            {}

            UnitClockDivisor unitClockDivisor = UnitClockDivisor::_64;
            bool channelAInverted = false;
            bool channelBInverted = false;
            Control control;
            Generator generatorA;
            Generator generatorB;
        };

        struct PinChannel
        {
            GpioPin& pinA = dummyPin;
            GpioPin& pinB = dummyPin;

            bool usesChannelA = false;
            bool usesChannelB = false;
        };

        class Channel
        {
        public:
            void Start(uint8_t dutyCycle);
            void Stop();
            void High();
            void Low();
        };

        Pwm(uint8_t aPwmIndex, PinChannel channel0, const Config& config = Config());
        Pwm(uint8_t aPwmIndex, PinChannel channel0, PinChannel channel1, const Config& config = Config());
        Pwm(uint8_t aPwmIndex, PinChannel channel0, PinChannel channel1, PinChannel channel2, const Config& config = Config());
        Pwm(uint8_t aPwmIndex, PinChannel channel0, PinChannel channel1, PinChannel channel2, PinChannel channel3,const Config& config = Config());

        ~Pwm();

        void SetBaseFrequency(uint32_t baseFrequency);
        void Start(uint8_t globalDutyCycle);
        void Stop();
        void DeadTime(uint8_t deadTime);

    private:
        struct PwmChannelType
        {
            __IO uint32_t  CTL;        /*!< PWM Control                            */
            __IO uint32_t  INTEN;      /*!< PWM Interrupt and Trigger Enable       */
            __IO uint32_t  RIS;        /*!< PWM Raw Interrupt Status               */
            __IO uint32_t  ISC;        /*!< PWM Interrupt Status and Clear         */
            __IO uint32_t  LOAD;       /*!< PWM Load                               */
            __IO uint32_t  COUNT;      /*!< PWM Counter                            */
            __IO uint32_t  CMPA;       /*!< PWM Compare A                          */
            __IO uint32_t  CMPB;       /*!< PWM Compare B                          */
            __IO uint32_t  GENA;       /*!< PWM Generator A Control                */
            __IO uint32_t  GENB;       /*!< PWM Generator B Control                */
            __IO uint32_t  DBCTL;      /*!< PWM Dead-Band Control                  */
            __IO uint32_t  DBRISE;     /*!< PWM Dead-Band Rising-Edge Delay        */
            __IO uint32_t  DBFALL;     /*!< PWM Dead-Band Falling-Edge-Delay       */
            __IO uint32_t  FLTSRC0;    /*!< PWM Fault Source 0                     */
            __IO uint32_t  FLTSRC1;    /*!< PWM Fault Source 1                     */
            __IO uint32_t  MINFLTPER;  /*!< PWM Minimum Fault Period               */    
        };

        static constexpr std::array<uint32_t, 4> peripheralPwmChannelOffsetArray = 
        {{
            0x00000040, /* Channel 0 */
            0x00000080, /* Channel 1 */
            0x000000C0, /* Channel 2 */
            0x00000100, /* Channel 3 */
        }};

        static constexpr PwmChannelType* const PwmChannel(uint32_t pwmBaseAddress, uint32_t channelIndex)
        {
            return reinterpret_cast<PwmChannelType* const>(pwmBaseAddress + peripheralPwmChannelOffsetArray[channelIndex]);
        }

        struct PeripheralChannels
        {
            explicit PeripheralChannels(PinChannel& pins, PinConfigPeripheral pinAConfig, PinConfigPeripheral pinBConfig, uint32_t pwmOffset, uint32_t index)
                : address(PwmChannel(pwmOffset, index))
            {
                index *= 2;

                if (pins.usesChannelA)
                {
                    a.Emplace(pins.pinA, pinAConfig);
                    enable |= 1 << index;
                }

                if (pins.usesChannelB)
                {
                    b.Emplace(pins.pinB, pinBConfig);
                    enable |= 1 << (index + 1);
                }
            }

            infra::Optional<PeripheralPin> a;
            infra::Optional<PeripheralPin> b;
            PwmChannelType* const address;
            uint32_t enable = 0;
        };

        uint8_t pwmIndex;
        const Config& config;
        infra::BoundedVector<PeripheralChannels>::WithMaxSize<4> channels;
        uint32_t peripheralFrequency;
        infra::MemoryRange<PWM0_Type* const> pwmArray;
        uint32_t baseFrequency;

    private:
        void Configure(const PeripheralChannels& channel);
        void SetBaseFrequency();
        void HandleInterrupt();
        void EnableClock(Config::UnitClockDivisor divisor);
        void DisableClock();
    };
}

#endif
