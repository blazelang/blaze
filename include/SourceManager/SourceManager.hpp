#pragma once

#include <string>
#include <vector>
#include <optional>
#include <unordered_map>

#include "SourceManager/ISourceManager.hpp"


class SourceManager : public ISourceManager {
    public:
        ~SourceManager() override;

        std::optional<FileID> loadFile(const std::string_view path) override;
        std::string_view getBuffer(FileID fileID) const override;
        std::string_view getPath(FileID fileID) const override;
        std::string_view getLine(FileID fileID, size_t offset) const override;

    private:
        struct SourceFile {
            std::string path;
            std::string source;
        };

        std::vector<SourceFile> m_sources;
        std::unordered_map<std::string, FileID> m_pathToID;
};
