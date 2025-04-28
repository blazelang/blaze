#pragma once

#include <vector>

#include "Diagnostics/IDiagnosticEngine.hpp"
#include "SourceManager/SourceManager.hpp"

class DiagnosticEngine : public IDiagnosticEngine {
    public:
        DiagnosticEngine(const SourceManager& sourceManager);
        ~DiagnosticEngine() override;

        bool hasWarnings() const { return m_warningCount > 0; }
        bool hasErrors() const { return m_errorCount > 0; }

        const std::vector<Diagnostic>& getDiagnostics() const { return m_diagnostics; }

        void report(Diagnostic diagnostic) override;
        void printDiagnostics();
        void clearDiagnostics();

    private:
        const SourceManager& m_sourceManager;
        std::vector<Diagnostic> m_diagnostics;
        size_t m_errorCount;
        size_t m_warningCount;

        std::string levelToString(DiagnosticLevel level) const;
};
