#include "Diagnostics/DiagnosticEngine.hpp"

#include <fmt/color.h>

#include "Diagnostics/Diagnostic.hpp"
#include "SourceManager/SourceManager.hpp"

DiagnosticEngine::DiagnosticEngine(ISourceManager& sourceManager) : m_sourceManager(sourceManager) {}

void DiagnosticEngine::addDiagnostic(std::unique_ptr<Diagnostic> diagnostic) {
    m_diagnostics.push_back(std::move(diagnostic));
}

void DiagnosticEngine::printDiagnostics() {
    for (auto& diagnostic : m_diagnostics) {
        Span span = diagnostic->span;
        fmt::print(fmt::bg(fmt::rgb(255, 96, 93)) | fmt::fg(fmt::color::black) | fmt::emphasis::bold, " Error[{}] ", diagnostic->code);
        fmt::println(" {}", diagnostic->message);
        fmt::println("--> {}:{}:{}", m_sourceManager.getPath(span.fileID), span.line, span.column);
    }
}
