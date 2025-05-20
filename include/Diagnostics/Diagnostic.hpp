#pragma once

#include "Diagnostics/DiagnosticID.hpp"
#include "SourceManager/SourceManager.hpp"

struct Span {
    SourceManager::FileID fileID;
    size_t line;
    size_t column;
};

struct Diagnostic {
    DiagnosticID id;
    std::string code;
    DiagnosticLevel level;
    std::string message;
    Span span;
};
