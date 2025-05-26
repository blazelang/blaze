#pragma once

#include <gmock/gmock.h>

#include "Diagnostics/DiagnosticEngine.hpp"
#include "Diagnostics/Diagnostic.hpp"

class MockDiagnosticEngine : public IDiagnosticEngine {
public:
    ~MockDiagnosticEngine() override {};

    MOCK_METHOD(void, addDiagnostic, (std::unique_ptr<Diagnostic> diagnostic), (override));
};
