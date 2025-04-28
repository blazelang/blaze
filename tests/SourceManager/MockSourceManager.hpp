#pragma once

#include <gmock/gmock.h>

#include "SourceManager/ISourceManager.hpp"

class MockSourceManager : public ISourceManager {
public:
    ~MockSourceManager() override {};

    MOCK_METHOD(std::optional<ISourceManager::FileID>, loadFile, (const std::string_view path), (override));

    MOCK_METHOD(std::string_view, getBuffer, (ISourceManager::FileID fileID), (const, override));

    MOCK_METHOD(std::string_view, getPath, (ISourceManager::FileID fileID), (const, override));

    MOCK_METHOD(std::string_view, getLine, (ISourceManager::FileID fileID, size_t offset), (const, override));
};
