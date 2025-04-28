#pragma once

#include <optional>
#include <string_view>

class ISourceManager {
    public:
        using FileID = int;

        virtual ~ISourceManager() = default;

        virtual std::optional<ISourceManager::FileID> loadFile(const std::string_view path) = 0;
        virtual std::string_view getBuffer(ISourceManager::FileID fileID) const = 0;
        virtual std::string_view getPath(ISourceManager::FileID fileID) const = 0;
        virtual std::string_view getLine(ISourceManager::FileID fileID, size_t offset) const = 0;
};
