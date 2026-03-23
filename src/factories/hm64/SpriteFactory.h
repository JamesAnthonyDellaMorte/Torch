#pragma once

#include "../BaseFactory.h"
#include "../ResourceType.h"
#include "../../types/RawBuffer.h"
#include "Companion.h"
#include "utils/Decompressor.h"

namespace HM64 {

class SpriteTextureBinaryExporter final : public BaseExporter {
  public:
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName,
                        YAML::Node& node, std::string* replacement) override {
        (void)entryName;
        (void)node;
        (void)replacement;

        auto writer = LUS::BinaryWriter();
        auto rawData = std::static_pointer_cast<RawBuffer>(data)->mBuffer;

        WriteHeader(writer, Torch::ResourceType::SpriteTexture, 0);
        writer.Write(static_cast<uint32_t>(rawData.size()));
        writer.Write(reinterpret_cast<char*>(rawData.data()), rawData.size());
        writer.Finish(write);
        return std::nullopt;
    }
};

class SpriteAssetsIndexBinaryExporter final : public BaseExporter {
  public:
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName,
                        YAML::Node& node, std::string* replacement) override {
        (void)entryName;
        (void)node;
        (void)replacement;

        auto writer = LUS::BinaryWriter();
        auto rawData = std::static_pointer_cast<RawBuffer>(data)->mBuffer;

        WriteHeader(writer, Torch::ResourceType::SpriteAssetsIndex, 0);
        writer.Write(static_cast<uint32_t>(rawData.size()));
        writer.Write(reinterpret_cast<char*>(rawData.data()), rawData.size());
        writer.Finish(write);
        return std::nullopt;
    }
};

class SpriteSpritesheetIndexBinaryExporter final : public BaseExporter {
  public:
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName,
                        YAML::Node& node, std::string* replacement) override {
        (void)entryName;
        (void)node;
        (void)replacement;

        auto writer = LUS::BinaryWriter();
        auto rawData = std::static_pointer_cast<RawBuffer>(data)->mBuffer;

        WriteHeader(writer, Torch::ResourceType::SpriteSpritesheetIndex, 0);
        writer.Write(static_cast<uint32_t>(rawData.size()));
        writer.Write(reinterpret_cast<char*>(rawData.data()), rawData.size());
        writer.Finish(write);
        return std::nullopt;
    }
};

class SpriteTextureFactory final : public BaseFactory {
  public:
    std::optional<std::shared_ptr<IParsedData>> parse(std::vector<uint8_t>& buffer, YAML::Node& data) override {
        auto [_, segment] = Decompressor::AutoDecode(data, buffer);
        return std::make_shared<RawBuffer>(segment.data, segment.size);
    }

  private:
    std::unordered_map<ExportType, std::shared_ptr<BaseExporter>> GetExporters() override {
        return {
            REGISTER(Binary, SpriteTextureBinaryExporter)
        };
    }
};

class SpriteAssetsIndexFactory final : public BaseFactory {
  public:
    std::optional<std::shared_ptr<IParsedData>> parse(std::vector<uint8_t>& buffer, YAML::Node& data) override {
        auto [_, segment] = Decompressor::AutoDecode(data, buffer);
        return std::make_shared<RawBuffer>(segment.data, segment.size);
    }

  private:
    std::unordered_map<ExportType, std::shared_ptr<BaseExporter>> GetExporters() override {
        return {
            REGISTER(Binary, SpriteAssetsIndexBinaryExporter)
        };
    }
};

class SpriteSpritesheetIndexFactory final : public BaseFactory {
  public:
    std::optional<std::shared_ptr<IParsedData>> parse(std::vector<uint8_t>& buffer, YAML::Node& data) override {
        auto [_, segment] = Decompressor::AutoDecode(data, buffer);
        return std::make_shared<RawBuffer>(segment.data, segment.size);
    }

  private:
    std::unordered_map<ExportType, std::shared_ptr<BaseExporter>> GetExporters() override {
        return {
            REGISTER(Binary, SpriteSpritesheetIndexBinaryExporter)
        };
    }
};

} // namespace HM64
