#pragma once

#include <gmock/gmock.h>

#include "Diagnostics/Diagnostic.hpp"
#include "Diagnostics/IDiagnosticEngine.hpp"

class MockDiagnosticEngine : public IDiagnosticEngine {
public:
    ~MockDiagnosticEngine() override {};

    MOCK_METHOD(void, report, (Diagnostic diagnostic), (override));
};
