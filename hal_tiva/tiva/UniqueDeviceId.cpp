#include "hal_tiva/tiva/UniqueDeviceId.hpp"

namespace hal
{
    infra::ConstByteRange UniqueDeviceId()
    {
        const uint8_t* base = reinterpret_cast<const uint8_t*>(0x400FE000);
        return infra::ConstByteRange(base, base + 8);
    }
}
