#pragma once

#include <string>
#include <vector>
#include <optional>
#include <unordered_map>

struct SourceFile {
    std::string path;
    std::string source;
};

class SourceManager {
    public:
        using FileID = int;

        std::optional<FileID> loadFile(const std::string& path);
        std::string_view getBuffer(FileID fileID) const;
        std::string_view getPath(FileID fileID) const;
        std::string_view getLine(FileID fileID, size_t offset) const;

    private:
        std::vector<SourceFile> m_files;
        std::unordered_map<std::string, FileID> m_pathToID;
};
