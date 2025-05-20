#pragma once

#include <memory>
#include <format>

#include "Diagnostics/Diagnostic.hpp"

class DiagnosticEngine;

class DiagnosticBuilder {
public:
    DiagnosticBuilder(DiagnosticEngine& engine, DiagnosticID id);
    ~DiagnosticBuilder() = default;

    template <typename... Args>
    DiagnosticBuilder& message(std::format_string<Args...> format, Args&&... args) {
        m_diagnostic->message = std::format(format, std::forward<Args>(args)...);
        return *this;
    }

    DiagnosticBuilder& span(Span span);

    void build();

private:
    DiagnosticEngine& m_engine;
    std::unique_ptr<Diagnostic> m_diagnostic;
};
