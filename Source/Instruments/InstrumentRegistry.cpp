#include "InstrumentRegistry.h"

#include <array>
#include <string_view>

namespace Aetheria::Instruments
{
namespace
{
constexpr LayerMask A = layerBit(Layer::A);
constexpr LayerMask B = layerBit(Layer::B);
constexpr LayerMask C = layerBit(Layer::C);
constexpr LayerMask D = layerBit(Layer::D);

constexpr ArticulationMask sustained =
    articulationBit(Articulation::Sustained);
constexpr ArticulationMask plucked =
    articulationBit(Articulation::Plucked);
constexpr ArticulationMask struck =
    articulationBit(Articulation::Struck);
constexpr ArticulationMask bowed =
    articulationBit(Articulation::Bowed);

constexpr bool isSourceKindCompatibleWithFamily(
    SourceKind sourceKind,
    Family family) noexcept
{
    switch (sourceKind)
    {
        case SourceKind::AcousticInstrument:
            return family != Family::Textures;

        case SourceKind::SynthInstrument:
            return true;

        case SourceKind::Texture:
        case SourceKind::NoiseSource:
            return family == Family::Textures;
    }

    return false;
}

constexpr PlayableSourceDefinition makeSource(
    PlayableSourceId id,
    std::string_view name,
    std::string_view key,
    SourceKind sourceKind,
    Family family,
    LayerMask layers,
    std::optional<NoteRange> triggerRange,

    ArticulationMask articulations,
    EngineType engine = EngineType::SamplePlayback,
    ReleaseMode release = ReleaseMode::Envelope,
    std::string_view sampleMap = {}) noexcept
{
    return {
        id, name, key, sourceKind, family, layers, triggerRange,
        articulations, engine, AssetStatus::Planned,
        release, sampleMap
    };
}

// IDs are permanent catalog identities. Never derive them from array position.
// Once published in saved projects, an ID must not be reassigned.
constexpr std::array sourceRegistry
{
    // Layer A: Sub / Body
    makeSource(0x00010001, "Octobass", "octobass",
        SourceKind::AcousticInstrument, Family::LowStrings, A, NoteRange{ 12, 48 }, bowed | sustained),
    makeSource(0x00010002, "Subcontrabass", "subcontrabass",
        SourceKind::AcousticInstrument, Family::LowStrings, A, NoteRange{ 12, 55 }, bowed | sustained),
    makeSource(0x00010003, "Hyperbass Flute", "hyperbass-flute",
        SourceKind::AcousticInstrument, Family::Flutes, A, NoteRange{ 24, 84 }, sustained),
    makeSource(0x00010004, "Earth Harp", "earth-harp",
        SourceKind::AcousticInstrument, Family::Harps, A | B, NoteRange{ 24, 96 }, plucked | sustained),
    makeSource(0x00010005, "Pipe Organ", "pipe-organ",
        SourceKind::AcousticInstrument, Family::Organs, A | B, NoteRange{ 24, 108 }, sustained),

    // Layer B: Mid / Harmony
    makeSource(0x00020001, "Concert Grand Piano", "concert-grand-piano",
        SourceKind::AcousticInstrument, Family::Pianos, B | C, NoteRange{ 21, 108 }, struck),
    makeSource(0x00020002, "Upright Piano", "upright-piano",
        SourceKind::AcousticInstrument, Family::Pianos, B | C, NoteRange{ 21, 108 }, struck),
    makeSource(0x00020003, "Nylon String Guitar", "nylon-string-guitar",
        SourceKind::AcousticInstrument, Family::Guitars, B | C, NoteRange{ 40, 88 }, plucked),
    makeSource(0x00020004, "Steel String Guitar", "steel-string-guitar",
        SourceKind::AcousticInstrument, Family::Guitars, B | C, NoteRange{ 40, 88 }, plucked),
    makeSource(0x00020005, "Concert Harp", "concert-harp",
        SourceKind::AcousticInstrument, Family::PluckedStrings, A | B | C, NoteRange{ 24, 103 }, plucked | sustained),
    makeSource(0x00020006, "Violin", "violin",
        SourceKind::AcousticInstrument, Family::BowedStrings, B | C, NoteRange{ 55, 103 }, bowed | sustained),
    makeSource(0x00020007, "Cello", "cello",
        SourceKind::AcousticInstrument, Family::BowedStrings, A | B | C, NoteRange{ 36, 88 }, bowed | sustained),
    makeSource(0x00020008, "Bassoon", "bassoon",
        SourceKind::AcousticInstrument, Family::Reeds, B | C, NoteRange{ 34, 75 }, sustained),

    // Layer C: Attack / Transient
    makeSource(0x00030001, "Timpani", "timpani",
        SourceKind::AcousticInstrument, Family::Percussion, C, NoteRange{ 36, 64 }, struck,
        EngineType::SamplePlayback, ReleaseMode::NaturalDecay),
    makeSource(0x00030002, "Concert Snare", "concert-snare",
        SourceKind::AcousticInstrument, Family::Percussion, C, std::nullopt, struck,
        EngineType::SamplePlayback, ReleaseMode::NaturalDecay)
};
} // namespace

std::span<const PlayableSourceDefinition> getPlayableSourceRegistry() noexcept
{
    return sourceRegistry;
}

const PlayableSourceDefinition* findPlayableSource(PlayableSourceId id) noexcept
{
    for (const auto& instrument : sourceRegistry)
        if (instrument.id == id)
            return &instrument;

    return nullptr;
}

const PlayableSourceDefinition* findPlayableSource(std::string_view stableKey) noexcept
{
    for (const auto& instrument : sourceRegistry)
        if (instrument.stableKey == stableKey)
            return &instrument;

    return nullptr;
}

bool isPlayableSourceCompatible(PlayableSourceId id, Layer layer) noexcept
{
    const auto* instrument = findPlayableSource(id);
    return instrument != nullptr
        && supportsLayer(instrument->compatibleLayers, layer);
}

bool isPlayableSourceDeclaredReady(PlayableSourceId id) noexcept
{
    const auto* instrument = findPlayableSource(id);

    return instrument != nullptr
        && instrument->assetStatus == AssetStatus::Ready;
}

bool validatePlayableSourceDefinitions(
    std::span<const PlayableSourceDefinition> sources) noexcept
{
    for (std::size_t i = 0; i < sources.size(); ++i)
    {
        const auto& item = sources[i];

        if (item.id == 0 || item.name.empty() || item.stableKey.empty())
            return false;

        if (item.compatibleLayers == 0
            || (item.compatibleLayers & 0xF0u) != 0)
            return false;

        if (static_cast<unsigned>(item.sourceKind)
                > static_cast<unsigned>(SourceKind::NoiseSource)
            || static_cast<unsigned>(item.family)
                > static_cast<unsigned>(Family::Textures)
            || static_cast<unsigned>(item.defaultEngine)
                > static_cast<unsigned>(EngineType::Granular)
            || static_cast<unsigned>(item.assetStatus)
                > static_cast<unsigned>(AssetStatus::Ready)
            || static_cast<unsigned>(item.releaseMode)
                > static_cast<unsigned>(ReleaseMode::NaturalDecay))
            return false;

        if (!isSourceKindCompatibleWithFamily(item.sourceKind, item.family))
            return false;

        constexpr ArticulationMask knownArticulations = 0x000001FFu;

        if ((item.articulations & ~knownArticulations) != 0)
            return false;

        if (item.triggerRange
            && (item.triggerRange->lowestMidiNote
                    > item.triggerRange->highestMidiNote
                || item.triggerRange->highestMidiNote > 127))
            return false;

        if (item.assetStatus == AssetStatus::Ready
            && item.defaultEngine == EngineType::SamplePlayback
            && item.sampleMapId.empty())
            return false;

        for (std::size_t j = i + 1; j < sources.size(); ++j)
        {
            if (item.id == sources[j].id
                || item.stableKey == sources[j].stableKey
                || item.name == sources[j].name)
                return false;
        }
    }

    return true;
}

bool validatePlayableSourceRegistry() noexcept
{
    return validatePlayableSourceDefinitions(sourceRegistry);
}

} // namespace Aetheria::Instruments