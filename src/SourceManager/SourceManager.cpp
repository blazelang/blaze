#include "SourceManager/SourceManager.hpp"

#include <fstream>
#include <iostream>
#include <optional>
#include <filesystem>
#include <sstream>
#include <string_view>

std::optional<int> SourceManager::loadFile(const std::string& path) {
    if (!std::filesystem::exists(path)) {
        return std::nullopt;
    }

    std::filesystem::path canonicalPath = std::filesystem::canonical(path);

    const auto& it = m_pathToID.find(canonicalPath.string());

    if (it != m_pathToID.end()) {
        return it->second;
    }

    std::ifstream file(canonicalPath, std::ios::binary);

    if (!file) {
        return std::nullopt;
    }

    std::stringstream sourceBuffer;

    sourceBuffer << file.rdbuf();

    SourceFile sourceFile = {
        .path = canonicalPath.string(),
        .source = sourceBuffer.str(),
    };

    FileID fileID = m_files.size();
    m_files.push_back(sourceFile);
    m_pathToID[canonicalPath.string()] = fileID;

    return fileID;
}

std::string_view SourceManager::getBuffer(FileID fileID)const {
    const SourceFile& sourceFile = m_files.at(fileID);
    return std::string_view(sourceFile.source);
}

std::string_view SourceManager::getPath(FileID fileID) const {
    const SourceFile& source = m_files.at(fileID);
    return std::string_view(source.path);
}

std::string_view SourceManager::getLine(FileID fileID, size_t offset) const {
    const SourceFile& sourceFile = m_files.at(fileID);
    const std::string& content = sourceFile.source;

    size_t end = content.find('\n', offset);
    if (end == std::string::npos) {
        end = content.size();
    }

    return std::string_view(content).substr(offset, end - offset);
}
