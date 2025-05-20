#pragma once

#include <string>

enum class DiagnosticID {
    UnterminatedBlockComment,

    // Number literal errors
    NumberLiteralInvalidSuffix,
    NumberLiteralEmptyDigits,
    NumberLiteralLeadingDot,
    NumberLiteralMultipleDots,
    NumberLiteralInvalidDigit,
    NumberLiteralEmptyExponent,
    NumberLiteralConsecutiveUnderscore,
    NumberLiteralLeadingUnderscore,
    NumberLiteralTrailingUnderscore,
    NumberLiteralUnderscoreBeforePrefix,
    NumberLiteralUnderscoreAfterPrefix,
    NumberLiteralUnderscoreBeforeDot,

    // Char
    EmptyChar,
    MultiChar,
    UnterminatedChar,
    InvalidEscapeChar,

    // String
    UnterminatedString,
    InvalidEscapeString,

    UnrecognizedSymbol,
};

enum class DiagnosticLevel {
    Note,
    Warning,
    Error,
    Fatal
};

struct DiagnosticInfo {
    DiagnosticLevel level;
    std::string code;
};

constexpr const DiagnosticInfo getDiagnosticInfo(DiagnosticID id) {
    switch (id) {
        case DiagnosticID::UnterminatedBlockComment:
            return { DiagnosticLevel::Error, "E1001" };

        // Number literal errors
        case DiagnosticID::NumberLiteralInvalidSuffix:
            return { DiagnosticLevel::Error, "E2001" };
        case DiagnosticID::NumberLiteralEmptyDigits:
            return { DiagnosticLevel::Error, "E2002" };
        case DiagnosticID::NumberLiteralLeadingDot:
            return { DiagnosticLevel::Error, "E2003" };
        case DiagnosticID::NumberLiteralMultipleDots:
            return { DiagnosticLevel::Error, "E2004" };
        case DiagnosticID::NumberLiteralInvalidDigit:
            return { DiagnosticLevel::Error, "E2005" };
        case DiagnosticID::NumberLiteralEmptyExponent:
            return { DiagnosticLevel::Error, "E2006" };
        case DiagnosticID::NumberLiteralConsecutiveUnderscore:
            return { DiagnosticLevel::Error, "E2007" };
        case DiagnosticID::NumberLiteralLeadingUnderscore:
            return { DiagnosticLevel::Error, "E2008" };
        case DiagnosticID::NumberLiteralTrailingUnderscore:
            return { DiagnosticLevel::Error, "E2009" };
        case DiagnosticID::NumberLiteralUnderscoreBeforePrefix:
            return { DiagnosticLevel::Error, "E2010" };
        case DiagnosticID::NumberLiteralUnderscoreAfterPrefix:
            return { DiagnosticLevel::Error, "E2011" };
        case DiagnosticID::NumberLiteralUnderscoreBeforeDot:
            return { DiagnosticLevel::Error, "E2012" };

        // Char literal errors
        case DiagnosticID::EmptyChar:
            return { DiagnosticLevel::Error, "E3001" };
        case DiagnosticID::UnterminatedChar:
            return { DiagnosticLevel::Error, "E3002" };
        case DiagnosticID::InvalidEscapeChar:
            return { DiagnosticLevel::Error, "E3003" };
        case DiagnosticID::MultiChar:
            return { DiagnosticLevel::Error, "E3004" };

        // String literal errors
        case DiagnosticID::UnterminatedString:
            return { DiagnosticLevel::Error, "E3101" };
        case DiagnosticID::InvalidEscapeString:
            return { DiagnosticLevel::Error, "E3102" };

        case DiagnosticID::UnrecognizedSymbol:
            return { DiagnosticLevel::Error, "E1010" };

        default:
            return { DiagnosticLevel::Error, "E0000" };
    }
}
