#pragma once

#include <string>
#include <vector>
#include <optional>
#include <unordered_map>

class ISourceManager {
public:
    using FileID = int;

    virtual ~ISourceManager() = default;

    virtual std::optional<ISourceManager::FileID> loadFile(const std::string_view path) = 0;
    virtual std::string_view getBuffer(ISourceManager::FileID fileID) const = 0;
    virtual std::string_view getPath(ISourceManager::FileID fileID) const = 0;
};

class SourceManager : public ISourceManager {
public:
    ~SourceManager() override;

    std::optional<FileID> loadFile(const std::string_view path) override;
    std::string_view getBuffer(FileID fileID) const override;
    std::string_view getPath(FileID fileID) const override;

private:
    struct SourceFile {
        std::string path;
        std::string source;
    };

    std::vector<SourceFile> m_sources;
    std::unordered_map<std::string, FileID> m_pathToID;
};
