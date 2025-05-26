#include "Diagnostics/DiagnosticBuilder.hpp"

#include <memory>

#include "Diagnostics/Diagnostic.hpp"
#include "Diagnostics/DiagnosticID.hpp"

DiagnosticBuilder::DiagnosticBuilder(DiagnosticID id, std::string message)
: m_diagnostic(std::make_unique<Diagnostic>()) {
    DiagnosticInfo info = getDiagnosticInfo(id);
    m_diagnostic->id = id;
    m_diagnostic->code = info.code;
    m_diagnostic->level= info.level;
    m_diagnostic->message = message;
}

DiagnosticBuilder& DiagnosticBuilder::span(Span span) {
    m_diagnostic->span = span;
    return *this;
}

std::unique_ptr<Diagnostic> DiagnosticBuilder::build() {
    return std::move(m_diagnostic);
}
