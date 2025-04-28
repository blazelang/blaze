#pragma once

#include "Diagnostics/Diagnostic.hpp"

class IDiagnosticEngine {
    public:
        virtual ~IDiagnosticEngine() {};

        virtual void report(Diagnostic diagnostic) = 0;
};
