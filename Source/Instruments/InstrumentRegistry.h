#pragma once

#include "InstrumentTypes.h"

#include <cstddef>
#include <span>

namespace Aetheria::Instruments
{
std::span<const PlayableSourceDefinition> getPlayableSourceRegistry() noexcept;

const PlayableSourceDefinition* findPlayableSource(PlayableSourceId id) noexcept;
const PlayableSourceDefinition* findPlayableSource(std::string_view stableKey) noexcept;

bool isPlayableSourceCompatible(PlayableSourceId id, Layer layer) noexcept;

// Reports catalog-declared readiness only.
// Does not prove that an engine or asset is loaded and operational.
bool isPlayableSourceDeclaredReady(PlayableSourceId id) noexcept;

// Validates catalog identity and metadata invariants.
// This does not load assets or prove that an audio engine works.
bool validatePlayableSourceDefinitions(
    std::span<const PlayableSourceDefinition> sources) noexcept;

bool validatePlayableSourceRegistry() noexcept;

} // namespace Aetheria::Instruments