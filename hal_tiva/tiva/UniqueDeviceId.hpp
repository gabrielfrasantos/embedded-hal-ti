#ifndef HAL_UNIQUE_DEVICE_ID_HPP
#define HAL_UNIQUE_DEVICE_ID_HPP

#include "infra/util/ByteRange.hpp"

namespace hal::tiva
{
    infra::ConstByteRange UniqueDeviceId();
}

#endif
