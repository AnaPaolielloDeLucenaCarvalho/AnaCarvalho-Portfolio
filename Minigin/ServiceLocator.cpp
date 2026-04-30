#include "ServiceLocator.h"

namespace portfolio
{
    std::unique_ptr<SoundSystem> ServiceLocator::_ss_instance{ std::make_unique<NullSoundSystem>() };
}