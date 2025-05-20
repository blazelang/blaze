#include "Diagnostics/DiagnosticBuilder.hpp"

#include <memory>

#include "Diagnostics/Diagnostic.hpp"
#include "Diagnostics/DiagnosticEngine.hpp"
#include "Diagnostics/DiagnosticID.hpp"

DiagnosticBuilder::DiagnosticBuilder(DiagnosticEngine& engine, DiagnosticID id)
:   m_engine(engine),
    m_diagnostic(std::make_unique<Diagnostic>())
{
    DiagnosticInfo info = getDiagnosticInfo(id);
    m_diagnostic->id = id;
    m_diagnostic->code = info.code;
    m_diagnostic->level= info.level;
}

DiagnosticBuilder& DiagnosticBuilder::span(Span span) {
    m_diagnostic->span = span;
    return *this;
}

void DiagnosticBuilder::build() {
    m_engine.addDiagnostic(std::move(m_diagnostic));
}
