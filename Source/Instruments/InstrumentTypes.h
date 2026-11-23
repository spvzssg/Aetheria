#pragma once

#include <cstdint>
#include <string_view>
#include <optional>

namespace Aetheria::Instruments
{
using PlayableSourceId = std::uint32_t;

enum class Layer : std::uint8_t
{
    A = 0,
    B = 1,
    C = 2,
    D = 3
};

using LayerMask = std::uint8_t;

constexpr bool isValidLayer(Layer layer) noexcept
{
    return static_cast<unsigned>(layer) <= static_cast<unsigned>(Layer::D);
}

constexpr LayerMask layerBit(Layer layer) noexcept
{
    return isValidLayer(layer)
        ? static_cast<LayerMask>(1u << static_cast<unsigned>(layer))
        : LayerMask{ 0 };
}

constexpr bool supportsLayer(LayerMask mask, Layer layer) noexcept
{
    return isValidLayer(layer)
        && (mask & layerBit(layer)) != 0;
}

enum class SourceKind : std::uint8_t
{
    AcousticInstrument,
    SynthInstrument,
    Texture,
    NoiseSource
};

enum class Family : std::uint8_t
{
    LowStrings,
    Flutes,
    Harps,
    Organs,
    Pianos,
    Guitars,
    PluckedStrings,
    BowedStrings,
    Reeds,
    Percussion,
    Brass,
    Winds,
    Textures
};

enum class EngineType : std::uint8_t
{
    SamplePlayback,
    PluckedStringPhysicalModel,
    Synthesizer,
    Granular
};

enum class AssetStatus : std::uint8_t
{
    Planned,
    Prototype,
    Ready
};

enum class ReleaseMode : std::uint8_t
{
    Envelope,
    OneShot,
    LoopUntilRelease,
    NaturalDecay
};

enum class Articulation : std::uint32_t
{
    None       = 0,
    Sustained  = 1u << 0,
    Plucked    = 1u << 1,
    Struck     = 1u << 2,
    Bowed      = 1u << 3,
    Muted      = 1u << 4,
    Harmonic   = 1u << 5,
    Tremolo    = 1u << 6,
    Flutter    = 1u << 7,
    Staccato   = 1u << 8
};

using ArticulationMask = std::uint32_t;

constexpr ArticulationMask articulationBit(Articulation value) noexcept
{
    return static_cast<ArticulationMask>(value);
}

struct NoteRange
{
    std::uint8_t lowestMidiNote = 0;
    std::uint8_t highestMidiNote = 127;
};

struct PlayableSourceDefinition
{
    PlayableSourceId id;
    std::string_view name;
    std::string_view stableKey;
    SourceKind sourceKind;
    Family family;
    LayerMask compatibleLayers;
    std::optional<NoteRange> triggerRange;
    ArticulationMask articulations;
    EngineType defaultEngine;
    AssetStatus assetStatus;
    ReleaseMode releaseMode;
    std::string_view sampleMapId;
};

} // namespace Aetheria::Instruments