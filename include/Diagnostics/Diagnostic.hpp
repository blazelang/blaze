#pragma once

#include <string>

enum class DiagnosticPhrase {
    Lexer,
    Parser
};

enum class DiagnosticLevel {
    Note,
    Warning,
    Error
};

struct SourcePosition {
    std::string_view fileName;
    std::pair<size_t, size_t> position;
};

struct Diagnostic {
    DiagnosticPhrase phrase;
    DiagnosticLevel lvl;
    SourcePosition position;
    std::string message;
};
