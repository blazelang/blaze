#pragma once

#include <gmock/gmock.h>

#include "Diagnostics/DiagnosticEngine.hpp"
#include "Diagnostics/DiagnosticID.hpp"

class MockDiagnosticEngine : public IDiagnosticEngine {
public:
    ~MockDiagnosticEngine() override {};

    MOCK_METHOD(DiagnosticBuilder, report, (DiagnosticID id), (override));
};
