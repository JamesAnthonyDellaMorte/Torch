#pragma once

#include "../BaseFactory.h"
#include "../ResourceType.h"
#include "types/RawBuffer.h"
#include "utils/Decompressor.h"

namespace HM64 {

class MapDataBinaryExporter final : public BaseExporter {
  public:
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName,
                        YAML::Node& node, std::string* replacement) override {
        (void)entryName;
        (void)node;
        (void)replacement;

        auto writer = LUS::BinaryWriter();
        auto raw = std::static_pointer_cast<RawBuffer>(data)->mBuffer;

        BaseExporter::WriteHeader(writer, Torch::ResourceType::MapData, 0);
        writer.Write(static_cast<uint32_t>(raw.size()));
        writer.Write(reinterpret_cast<char*>(raw.data()), raw.size());
        writer.Finish(write);
        return std::nullopt;
    }
};

class MapDataFactory final : public BaseFactory {
  public:
    std::optional<std::shared_ptr<IParsedData>> parse(std::vector<uint8_t>& buffer, YAML::Node& node) override {
        auto [_, segment] = Decompressor::AutoDecode(node, buffer);
        return std::make_shared<RawBuffer>(segment.data, segment.size);
    }

  private:
    std::unordered_map<ExportType, std::shared_ptr<BaseExporter>> GetExporters() override {
        return { REGISTER(Binary, MapDataBinaryExporter) };
    }
};

} // namespace HM64
