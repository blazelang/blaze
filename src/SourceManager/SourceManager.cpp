#include "SourceManager/SourceManager.hpp"

#include <fstream>
#include <iostream>
#include <optional>
#include <filesystem>
#include <sstream>
#include <string_view>

std::optional<ISourceManager::FileID> SourceManager::loadFile(const std::string_view path) {
    if (!std::filesystem::exists(path)) {
        return std::nullopt;
    }
    std::filesystem::path canonicalPath = std::filesystem::canonical(path);

    const auto& pathToIDIt = m_pathToID.find(canonicalPath.string());
    if (pathToIDIt != m_pathToID.end()) {
        return pathToIDIt->second;
    }

    std::ifstream file(canonicalPath, std::ios::binary);
    if (!file) {
        return std::nullopt;
    }

    std::stringstream sourceBuffer;
    sourceBuffer << file.rdbuf();

    SourceManager::SourceFile sourceFile = { .path = canonicalPath.string(), .source = sourceBuffer.str() };
    // Find the sources size before push back to sourceFile for index
    ISourceManager::FileID fileID = m_sources.size();

    m_sources.push_back(sourceFile);
    m_pathToID[canonicalPath.string()] = fileID;

    return fileID;
}

std::string_view SourceManager::getBuffer(FileID fileID) const {
    const SourceManager::SourceFile& sourceFile = m_sources.at(fileID);
    return std::string_view(sourceFile.source);
}

std::string_view SourceManager::getPath(FileID fileID) const {
    const SourceManager::SourceManager::SourceFile& source = m_sources.at(fileID);
    return std::string_view(source.path);
}

std::string_view SourceManager::getLine(FileID fileID, size_t offset) const {
    const SourceManager::SourceFile& sourceFile = m_sources.at(fileID);
    const std::string_view content = sourceFile.source;

    size_t end = content.find('\n', offset);
    if (end == std::string::npos) {
        end = content.size();
    }

    return std::string_view(content).substr(offset, end - offset);
}

SourceManager::~SourceManager() {
    m_sources.clear();
    m_pathToID.clear();
}
