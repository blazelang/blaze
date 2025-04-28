#include "Diagnostics/DiagnosticEngine.hpp"

#include <iostream>
#include <format>

DiagnosticEngine::DiagnosticEngine(const SourceManager& sourceManager) :
    m_sourceManager(sourceManager),
    m_warningCount(0),
    m_errorCount(0) {}

void DiagnosticEngine::report(Diagnostic diagnostic) {
    if (diagnostic.lvl == DiagnosticLevel::Error) {
        m_errorCount += 1;
    } else if (diagnostic.lvl == DiagnosticLevel::Warning) {
        m_warningCount += 1;
    }
    m_diagnostics.push_back(diagnostic);
}

void DiagnosticEngine::printDiagnostics() {
    for (const auto& diagnostic : m_diagnostics) {
        std::cout << std::format("{}", diagnostic.message) << '\n';
    }
}

void DiagnosticEngine::clearDiagnostics() {
    m_diagnostics.clear();
    m_warningCount = 0;
    m_errorCount = 0;
}

DiagnosticEngine::~DiagnosticEngine() {
    m_diagnostics.clear();
}
