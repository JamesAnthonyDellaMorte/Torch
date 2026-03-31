#pragma once

#include "../BaseFactory.h"
#include "../ResourceType.h"
#include "../../types/RawBuffer.h"
#include "Companion.h"
#include "utils/Decompressor.h"

namespace HM64 {

class DialogueIndexData : public IParsedData {
  public:
    std::vector<uint32_t> Offsets;
};

class DialogueDataBinaryExporter final : public BaseExporter {
  public:
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName,
                        YAML::Node& node, std::string* replacement) override {
        (void)entryName;
        (void)node;
        (void)replacement;

        auto writer = LUS::BinaryWriter();
        auto rawData = std::static_pointer_cast<RawBuffer>(data)->mBuffer;

        WriteHeader(writer, Torch::ResourceType::DialogueData, 0);
        writer.Write(static_cast<uint32_t>(rawData.size()));
        writer.Write(reinterpret_cast<char*>(rawData.data()), rawData.size());
        writer.Finish(write);
        return std::nullopt;
    }
};

class DialogueIndexBinaryExporter final : public BaseExporter {
  public:
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName,
                        YAML::Node& node, std::string* replacement) override {
        (void)entryName;
        (void)node;
        (void)replacement;

        auto writer = LUS::BinaryWriter();
        auto indexData = std::static_pointer_cast<DialogueIndexData>(data);

        WriteHeader(writer, Torch::ResourceType::DialogueIndex, 0);
        writer.Write(static_cast<uint32_t>(indexData->Offsets.size()));
        for (const auto value : indexData->Offsets) {
            writer.Write(value);
        }
        writer.Finish(write);
        return std::nullopt;
    }
};

class DialogueDataFactory final : public BaseFactory {
  public:
    std::optional<std::shared_ptr<IParsedData>> parse(std::vector<uint8_t>& buffer, YAML::Node& data) override {
        auto [_, segment] = Decompressor::AutoDecode(data, buffer);
        return std::make_shared<RawBuffer>(segment.data, segment.size);
    }

  private:
    std::unordered_map<ExportType, std::shared_ptr<BaseExporter>> GetExporters() override {
        return {
            REGISTER(Binary, DialogueDataBinaryExporter)
        };
    }
};

class DialogueIndexFactory final : public BaseFactory {
  public:
    std::optional<std::shared_ptr<IParsedData>> parse(std::vector<uint8_t>& buffer, YAML::Node& data) override {
        auto [_, segment] = Decompressor::AutoDecode(data, buffer);
        if ((segment.size % sizeof(uint32_t)) != 0) {
            throw std::runtime_error("HM64 dialogue index buffer size is not aligned to 32-bit entries");
        }

        auto parsed = std::make_shared<DialogueIndexData>();
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

  private:
    std::unordered_map<ExportType, std::shared_ptr<BaseExporter>> GetExporters() override {
        return {
            REGISTER(Binary, DialogueIndexBinaryExporter)
        };
    }
};

} // namespace HM64
