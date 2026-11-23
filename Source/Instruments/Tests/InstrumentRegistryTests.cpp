#include "Instruments/InstrumentRegistry.h"

#include <array>
#include <iostream>
#include <initializer_list>
#include <string_view>

using namespace Aetheria::Instruments;

namespace
{
struct ExpectedSource
{
    PlayableSourceId id;
    std::string_view stableKey;
    LayerMask layers;
};

constexpr std::array expectedSources
{
    ExpectedSource{ 0x00010001, "octobass", layerBit(Layer::A) },
    ExpectedSource{ 0x00010002, "subcontrabass", layerBit(Layer::A) },
    ExpectedSource{ 0x00010003, "hyperbass-flute", layerBit(Layer::A) },
    ExpectedSource{ 0x00010004, "earth-harp", layerBit(Layer::A) | layerBit(Layer::B) },
    ExpectedSource{ 0x00010005, "pipe-organ", layerBit(Layer::A) | layerBit(Layer::B) },

    ExpectedSource{ 0x00020001, "concert-grand-piano", layerBit(Layer::B) | layerBit(Layer::C) },
    ExpectedSource{ 0x00020002, "upright-piano", layerBit(Layer::B) | layerBit(Layer::C) },
    ExpectedSource{ 0x00020003, "nylon-string-guitar", layerBit(Layer::B) | layerBit(Layer::C) },
    ExpectedSource{ 0x00020004, "steel-string-guitar", layerBit(Layer::B) | layerBit(Layer::C) },
    ExpectedSource{ 0x00020005, "concert-harp", layerBit(Layer::A) | layerBit(Layer::B) | layerBit(Layer::C) },
    ExpectedSource{ 0x00020006, "violin", layerBit(Layer::B) | layerBit(Layer::C) },
    ExpectedSource{ 0x00020007, "cello", layerBit(Layer::A) | layerBit(Layer::B) | layerBit(Layer::C) },
    ExpectedSource{ 0x00020008, "bassoon", layerBit(Layer::B) | layerBit(Layer::C) },

    ExpectedSource{ 0x00030001, "timpani", layerBit(Layer::C) },
    ExpectedSource{ 0x00030002, "concert-snare", layerBit(Layer::C) }
};

int failures = 0;

void check(bool condition, std::string_view description)
{
    if (!condition)
    {
        std::cerr << "FAIL: " << description << '\n';
        ++failures;
    }
}

void testRegistryValidation()
{
    check(validatePlayableSourceRegistry(),
          "catalog metadata validation");

    check(getPlayableSourceRegistry().size() == expectedSources.size(),
          "catalog contains exactly 15 sources");
}

void testIdentityAndLayerCompatibility()
{
    for (const auto& expected : expectedSources)
    {
        const auto* byId = findPlayableSource(expected.id);
        const auto* byKey = findPlayableSource(expected.stableKey);

        check(byId != nullptr, "numeric ID resolves");
        check(byKey != nullptr, "stable key resolves");

        if (byId == nullptr || byKey == nullptr)
            continue;

        check(byId == byKey, "ID and stable key resolve to same entry");
        check(byId->stableKey == expected.stableKey,
              "stable key matches permanent catalog identity");
        check(byId->compatibleLayers == expected.layers,
              "compatible layer mask matches expected catalog");

        for (const auto layer : { Layer::A, Layer::B, Layer::C, Layer::D })
        {
            const bool expectedCompatible =
                supportsLayer(expected.layers, layer);

            check(isPlayableSourceCompatible(expected.id, layer)
                      == expectedCompatible,
                  "layer compatibility matches expected mask");
        }
    }
}

void testUnknownLookups()
{
    check(findPlayableSource(0) == nullptr,
          "unknown numeric ID returns nullptr");

    check(findPlayableSource(0xFFFFFFFFu) == nullptr,
          "unregistered numeric ID returns nullptr");

    check(findPlayableSource("unknown-source") == nullptr,
          "unknown stable key returns nullptr");

    check(!isPlayableSourceCompatible(0xFFFFFFFFu, Layer::A),
          "unknown source is not layer-compatible");

    check(!isPlayableSourceCompatible(0x00010001, static_cast<Layer>(255)),
          "invalid layer is rejected");
}

void testDeclaredReadiness()
{
    for (const auto& source : getPlayableSourceRegistry())
    {
        check(source.assetStatus == AssetStatus::Planned,
              "current catalog source remains Planned");

        check(!isPlayableSourceDeclaredReady(source.id),
              "Planned source is not declared ready");
    }

    check(!isPlayableSourceDeclaredReady(0xFFFFFFFFu),
          "unknown source is not declared ready");
}

void testExistingTriggerRanges()
{
    const auto* timpani = findPlayableSource("timpani");
    const auto* snare = findPlayableSource("concert-snare");

    check(timpani != nullptr && timpani->triggerRange.has_value(),
          "Timpani trigger range remains declared");

    if (timpani != nullptr && timpani->triggerRange.has_value())
    {
        check(timpani->triggerRange->lowestMidiNote == 36
                  && timpani->triggerRange->highestMidiNote == 64,
              "Timpani MIDI range remains 36-64");
    }

    check(snare != nullptr && !snare->triggerRange.has_value(),
          "Concert Snare trigger range remains unspecified");
}
PlayableSourceDefinition makeValidDefinition()
{
    return {
        1,
        "Valid Source",
        "valid-source",
        SourceKind::AcousticInstrument,
        Family::Pianos,
        layerBit(Layer::B),
        NoteRange{ 21, 108 },
        articulationBit(Articulation::Struck),
        EngineType::Synthesizer,
        AssetStatus::Planned,
        ReleaseMode::Envelope,
        {}
    };
}

bool validateSingle(PlayableSourceDefinition definition)
{
    const std::array definitions{ definition };
    return validatePlayableSourceDefinitions(definitions);
}

void testValidatorIdentityAndMetadataFailures()
{
    check(validatePlayableSourceDefinitions(
              std::span<const PlayableSourceDefinition>{}),
          "empty definition collection is structurally valid");

    check(validateSingle(makeValidDefinition()),
          "valid single definition is accepted");

    auto item = makeValidDefinition();
    item.id = 0;
    check(!validateSingle(item), "zero ID is rejected");

    item = makeValidDefinition();
    item.name = {};
    check(!validateSingle(item), "empty display name is rejected");

    item = makeValidDefinition();
    item.stableKey = {};
    check(!validateSingle(item), "empty stable key is rejected");

    item = makeValidDefinition();
    item.compatibleLayers = 0;
    check(!validateSingle(item), "empty layer mask is rejected");

    item = makeValidDefinition();
    item.compatibleLayers = 0x10u;
    check(!validateSingle(item), "layer mask with unsupported high bits is rejected");

    item = makeValidDefinition();
    item.articulations = 0x80000000u;
    check(!validateSingle(item), "unknown articulation bits are rejected");

    item = makeValidDefinition();
    item.triggerRange = NoteRange{ 80, 40 };
    check(!validateSingle(item), "reversed trigger range is rejected");

    item = makeValidDefinition();
    item.triggerRange.reset();
    check(validateSingle(item), "unspecified trigger range is accepted");

    item = makeValidDefinition();
    item.articulations = 0;
    check(validateSingle(item), "empty articulation mask is accepted");
}

void testValidatorEnumFailures()
{
    auto item = makeValidDefinition();
    item.sourceKind = static_cast<SourceKind>(255);
    check(!validateSingle(item), "invalid source-kind enum is rejected");

    item = makeValidDefinition();
    item.family = static_cast<Family>(255);
    check(!validateSingle(item), "invalid family enum is rejected");

    item = makeValidDefinition();
    item.defaultEngine = static_cast<EngineType>(255);
    check(!validateSingle(item), "invalid engine enum is rejected");

    item = makeValidDefinition();
    item.assetStatus = static_cast<AssetStatus>(255);
    check(!validateSingle(item), "invalid asset-status enum is rejected");

    item = makeValidDefinition();
    item.releaseMode = static_cast<ReleaseMode>(255);
    check(!validateSingle(item), "invalid release-mode enum is rejected");
}

void testValidatorDuplicateDetection()
{
    auto first = makeValidDefinition();
    auto second = makeValidDefinition();

    second.id = 2;
    second.name = "Second Source";
    second.stableKey = "second-source";

    std::array definitions{ first, second };
    check(validatePlayableSourceDefinitions(definitions),
          "distinct definitions are accepted");

    definitions = { first, second };
    definitions[1].id = definitions[0].id;
    check(!validatePlayableSourceDefinitions(definitions),
          "duplicate numeric IDs are rejected");

    definitions = { first, second };
    definitions[1].stableKey = definitions[0].stableKey;
    check(!validatePlayableSourceDefinitions(definitions),
          "duplicate stable keys are rejected");

    definitions = { first, second };
    definitions[1].name = definitions[0].name;
    check(!validatePlayableSourceDefinitions(definitions),
          "duplicate display names are rejected");
}

void testValidatorSourceKindFamilyCompatibility()
{
    constexpr std::array sourceKinds{
        SourceKind::AcousticInstrument,
        SourceKind::SynthInstrument,
        SourceKind::Texture,
        SourceKind::NoiseSource
    };

    constexpr std::array families{
        Family::LowStrings,
        Family::Flutes,
        Family::Harps,
        Family::Organs,
        Family::Pianos,
        Family::Guitars,
        Family::PluckedStrings,
        Family::BowedStrings,
        Family::Reeds,
        Family::Percussion,
        Family::Brass,
        Family::Winds,
        Family::Textures
    };

    for (const auto kind : sourceKinds)
    {
        for (const auto family : families)
        {
            auto item = makeValidDefinition();
            item.sourceKind = kind;
            item.family = family;

            const bool expectedValid =
                kind == SourceKind::SynthInstrument
                || (kind == SourceKind::AcousticInstrument
                    && family != Family::Textures)
                || ((kind == SourceKind::Texture
                     || kind == SourceKind::NoiseSource)
                    && family == Family::Textures);

            check(validateSingle(item) == expectedValid,
                  "source-kind and family compatibility follows policy");
        }
    }
}

void testValidatorReadySampleMapInvariant()
{
    auto item = makeValidDefinition();
    item.assetStatus = AssetStatus::Ready;
    item.defaultEngine = EngineType::SamplePlayback;
    item.sampleMapId = {};

    check(!validateSingle(item),
          "Ready sample-playback source requires a sample map");

    item.sampleMapId = "valid-sample-map";
    check(validateSingle(item),
          "Ready sample-playback source with sample map is accepted");

    item.defaultEngine = EngineType::Synthesizer;
    item.sampleMapId = {};
    check(validateSingle(item),
          "Ready non-sample engine does not require a sample map");
}

}

int main()

{
    testRegistryValidation();
    testIdentityAndLayerCompatibility();
    testUnknownLookups();
    testDeclaredReadiness();
    testExistingTriggerRanges();
    testValidatorIdentityAndMetadataFailures();
    testValidatorEnumFailures();
    testValidatorDuplicateDetection();
    testValidatorSourceKindFamilyCompatibility();
    testValidatorReadySampleMapInvariant();

    if (failures != 0)
    {
        std::cerr << "\nRegistry tests failed: " << failures << '\n';
        return 1;
    }

    std::cout << "All AETHERIA instrument registry tests passed.\n";
    return 0;
}