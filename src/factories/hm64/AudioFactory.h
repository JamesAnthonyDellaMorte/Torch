#pragma once

#include "../BaseFactory.h"
#include "../ResourceType.h"
#include "types/RawBuffer.h"
#include "utils/Decompressor.h"

namespace HM64 {

inline ExportResult ExportAudioRaw(std::ostream& write, std::shared_ptr<IParsedData> raw,
                                   Torch::ResourceType resourceType) {
    auto writer = LUS::BinaryWriter();
    auto data = std::static_pointer_cast<RawBuffer>(raw)->mBuffer;

    BaseExporter::WriteHeader(writer, resourceType, 0);
    writer.Write(static_cast<uint32_t>(data.size()));
    writer.Write(reinterpret_cast<char*>(data.data()), data.size());
    writer.Finish(write);
    return std::nullopt;
}

class AudioSequenceBinaryExporter final : public BaseExporter {
  public:
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName,
                        YAML::Node& node, std::string* replacement) override {
        (void)entryName;
        (void)node;
        (void)replacement;
        return ExportAudioRaw(write, data, Torch::ResourceType::AudioSequence);
    }
};

class AudioPointerBankBinaryExporter final : public BaseExporter {
  public:
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName,
                        YAML::Node& node, std::string* replacement) override {
        (void)entryName;
        (void)node;
        (void)replacement;
        return ExportAudioRaw(write, data, Torch::ResourceType::AudioPointerBank);
    }
};

class AudioWaveTableBinaryExporter final : public BaseExporter {
  public:
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName,
                        YAML::Node& node, std::string* replacement) override {
        (void)entryName;
        (void)node;
        (void)replacement;
        return ExportAudioRaw(write, data, Torch::ResourceType::AudioWaveTable);
    }
};

class AudioSequenceFactory final : public BaseFactory {
  public:
    std::optional<std::shared_ptr<IParsedData>> parse(std::vector<uint8_t>& buffer, YAML::Node& node) override {
        auto [_, segment] = Decompressor::AutoDecode(node, buffer);
        return std::make_shared<RawBuffer>(segment.data, segment.size);
    }

  private:
    std::unordered_map<ExportType, std::shared_ptr<BaseExporter>> GetExporters() override {
        return { REGISTER(Binary, AudioSequenceBinaryExporter) };
    }
};

class AudioPointerBankFactory final : public BaseFactory {
  public:
    std::optional<std::shared_ptr<IParsedData>> parse(std::vector<uint8_t>& buffer, YAML::Node& node) override {
        auto [_, segment] = Decompressor::AutoDecode(node, buffer);
        return std::make_shared<RawBuffer>(segment.data, segment.size);
    }

  private:
    std::unordered_map<ExportType, std::shared_ptr<BaseExporter>> GetExporters() override {
        return { REGISTER(Binary, AudioPointerBankBinaryExporter) };
    }
};

class AudioWaveTableFactory final : public BaseFactory {
  public:
    std::optional<std::shared_ptr<IParsedData>> parse(std::vector<uint8_t>& buffer, YAML::Node& node) override {
        auto [_, segment] = Decompressor::AutoDecode(node, buffer);
        return std::make_shared<RawBuffer>(segment.data, segment.size);
    }

  private:
    std::unordered_map<ExportType, std::shared_ptr<BaseExporter>> GetExporters() override {
        return { REGISTER(Binary, AudioWaveTableBinaryExporter) };
    }
};

} // namespace HM64
