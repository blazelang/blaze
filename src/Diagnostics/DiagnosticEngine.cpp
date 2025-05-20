#include "Diagnostics/DiagnosticEngine.hpp"

#include <fmt/color.h>

#include "Diagnostics/Diagnostic.hpp"
#include "Diagnostics/DiagnosticID.hpp"
#include "Diagnostics/DiagnosticBuilder.hpp"
#include "SourceManager/SourceManager.hpp"

DiagnosticEngine::DiagnosticEngine(ISourceManager& sourceManager) : m_sourceManager(sourceManager) {}

DiagnosticBuilder DiagnosticEngine::report(DiagnosticID id) {
    return DiagnosticBuilder(*this, id);
}

void DiagnosticEngine::addDiagnostic(std::unique_ptr<Diagnostic> diagnostic) {
    m_diagnostics.push_back(std::move(diagnostic));
}

void DiagnosticEngine::printDiagnostics() {
    for (auto& diagnostic : m_diagnostics) {
        fmt::print(fmt::bg(fmt::rgb(255, 96, 93)) | fmt::fg(fmt::color::black) | fmt::emphasis::bold, " Error[{}] ", diagnostic->code);
        fmt::println(" {}", diagnostic->message);
    }
}
