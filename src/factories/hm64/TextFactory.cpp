#include "TextFactory.h"

#include "../ResourceType.h"
#include "Companion.h"
#include "utils/Decompressor.h"

namespace HM64 {

ExportResult TextDataBinaryExporter::Export(std::ostream& write, std::shared_ptr<IParsedData> raw, std::string& entryName,
                                            YAML::Node& node, std::string* replacement) {
    (void)entryName;
    (void)node;
    (void)replacement;

    auto writer = LUS::BinaryWriter();
    auto data = std::static_pointer_cast<RawBuffer>(raw)->mBuffer;

    WriteHeader(writer, Torch::ResourceType::TextData, 0);
    writer.Write(static_cast<uint32_t>(data.size()));
    writer.Write(reinterpret_cast<char*>(data.data()), data.size());
    writer.Finish(write);
    return std::nullopt;
}

ExportResult TextIndexBinaryExporter::Export(std::ostream& write, std::shared_ptr<IParsedData> raw, std::string& entryName,
                                             YAML::Node& node, std::string* replacement) {
    (void)entryName;
    (void)node;
    (void)replacement;

    auto writer = LUS::BinaryWriter();
    auto indexData = std::static_pointer_cast<TextIndexData>(raw);

    WriteHeader(writer, Torch::ResourceType::TextIndex, 0);
    writer.Write(static_cast<uint32_t>(indexData->Offsets.size()));
    for (const auto value : indexData->Offsets) {
        writer.Write(value);
    }
    writer.Finish(write);
    return std::nullopt;
}

std::optional<std::shared_ptr<IParsedData>> TextDataFactory::parse(std::vector<uint8_t>& buffer, YAML::Node& node) {
    auto [_, segment] = Decompressor::AutoDecode(node, buffer);
    return std::make_shared<RawBuffer>(segment.data, segment.size);
}

std::optional<std::shared_ptr<IParsedData>> TextIndexFactory::parse(std::vector<uint8_t>& buffer, YAML::Node& node) {
    auto [_, segment] = Decompressor::AutoDecode(node, buffer);
    if ((segment.size % sizeof(uint32_t)) != 0) {
        throw std::runtime_error("HM64 text index buffer size is not aligned to 32-bit entries");
    }

    auto parsed = std::make_shared<TextIndexData>();
    parsed->Offsets.reserve(segment.size / sizeof(uint32_t));

    for (size_t i = 0; i < segment.size; i += sizeof(uint32_t)) {
        const auto value = (static_cast<uint32_t>(segment.data[i]) << 24) |
                           (static_cast<uint32_t>(segment.data[i + 1]) << 16) |
                           (static_cast<uint32_t>(segment.data[i + 2]) << 8) |
                           static_cast<uint32_t>(segment.data[i + 3]);
        parsed->Offsets.push_back(value);
    }

    return parsed;
}

} // namespace HM64
