#pragma once

#include <memory>

#include <fmt/format.h>

#include "Diagnostics/Diagnostic.hpp"
#include "SourceManager/SourceManager.hpp"

class IDiagnosticEngine {
public:
    virtual ~IDiagnosticEngine() = default;

    virtual void addDiagnostic(std::unique_ptr<Diagnostic> diagnostic) = 0;
};

class DiagnosticEngine : public IDiagnosticEngine {
public:
    DiagnosticEngine(ISourceManager& sourceManager);
    ~DiagnosticEngine() {};

    bool hasErrors() const { return m_errorCount > 0; }

    void addDiagnostic(std::unique_ptr<Diagnostic> diagnostic) override;

    void printDiagnostics();

private:
    ISourceManager& m_sourceManager;
    size_t m_errorCount = 0;
    size_t m_warningCount = 0;
    std::vector<std::unique_ptr<Diagnostic>> m_diagnostics;
};
