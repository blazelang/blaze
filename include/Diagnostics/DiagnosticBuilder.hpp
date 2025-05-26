#pragma once

#include <memory>

#include "Diagnostics/Diagnostic.hpp"

class DiagnosticBuilder {
public:
    DiagnosticBuilder(DiagnosticID id, std::string message);
    ~DiagnosticBuilder() = default;

    DiagnosticBuilder& span(Span span);

    std::unique_ptr<Diagnostic> build();

private:
    std::unique_ptr<Diagnostic> m_diagnostic;
};
