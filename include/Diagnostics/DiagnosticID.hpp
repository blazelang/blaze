#pragma once

#include <string>

enum class DiagnosticID {
    BlockCommentUnterminated,

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
    CharEmpty,
    CharMultiCodepoint,
    CharUnterminated,
    CharEscapeUnknown,
    CharEscapeHexTooShort,
    CharEscapeInvalidHexDigit,
    CharEscapeHexOutOfRange,
    CharEscapeMissingUnicodeBrace,
    CharEscapeInvalidUnicodeDigit,
    CharEscapeUnterminatedUnicode,
    CharEscapeEmptyUnicode,
    CharEscapeOverlongUnicode,
    CharEscapeInvalidUnicodeRange,

    // String
    StringUnterminated,
    StringEscapeUnknown,
    StringEscapeHexTooShort,
    StringEscapeInvalidHexDigit,
    StringEscapeHexOutOfRange,
    StringEscapeMissingUnicodeBrace,
    StringEscapeInvalidUnicodeDigit,
    StringEscapeUnterminatedUnicode,
    StringEscapeEmptyUnicode,
    StringEscapeOverlongUnicode,
    StringEscapeInvalidUnicodeRange,

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

constexpr DiagnosticInfo getDiagnosticInfo(DiagnosticID id) {
    switch (id) {
        case DiagnosticID::BlockCommentUnterminated: return { DiagnosticLevel::Error, "E1001" };

        // Number literal errors
        case DiagnosticID::NumberLiteralInvalidSuffix: return { DiagnosticLevel::Error, "E2001" };
        case DiagnosticID::NumberLiteralEmptyDigits: return { DiagnosticLevel::Error, "E2002" };
        case DiagnosticID::NumberLiteralLeadingDot: return { DiagnosticLevel::Error, "E2003" };
        case DiagnosticID::NumberLiteralMultipleDots: return { DiagnosticLevel::Error, "E2004" };
        case DiagnosticID::NumberLiteralInvalidDigit: return { DiagnosticLevel::Error, "E2005" };
        case DiagnosticID::NumberLiteralEmptyExponent: return { DiagnosticLevel::Error, "E2006" };
        case DiagnosticID::NumberLiteralConsecutiveUnderscore: return { DiagnosticLevel::Error, "E2007" };
        case DiagnosticID::NumberLiteralLeadingUnderscore: return { DiagnosticLevel::Error, "E2008" };
        case DiagnosticID::NumberLiteralTrailingUnderscore: return { DiagnosticLevel::Error, "E2009" };
        case DiagnosticID::NumberLiteralUnderscoreBeforePrefix: return { DiagnosticLevel::Error, "E2010" };
        case DiagnosticID::NumberLiteralUnderscoreAfterPrefix: return { DiagnosticLevel::Error, "E2011" };
        case DiagnosticID::NumberLiteralUnderscoreBeforeDot: return { DiagnosticLevel::Error, "E2012" };

        // Char literal errors
        case DiagnosticID::CharEmpty: return { DiagnosticLevel::Error, "E3001" };
        case DiagnosticID::CharMultiCodepoint: return { DiagnosticLevel::Error, "E3002" };
        case DiagnosticID::CharUnterminated: return { DiagnosticLevel::Error, "E3003" };
        case DiagnosticID::CharEscapeUnknown: return { DiagnosticLevel::Error, "E3004" };
        case DiagnosticID::CharEscapeHexTooShort: return { DiagnosticLevel::Error, "E3005" };
        case DiagnosticID::CharEscapeInvalidHexDigit: return { DiagnosticLevel::Error, "E3006" };
        case DiagnosticID::CharEscapeHexOutOfRange: return { DiagnosticLevel::Error, "E3007" };
        case DiagnosticID::CharEscapeMissingUnicodeBrace: return { DiagnosticLevel::Error, "E3008" };
        case DiagnosticID::CharEscapeInvalidUnicodeDigit: return { DiagnosticLevel::Error, "E3009" };
        case DiagnosticID::CharEscapeUnterminatedUnicode: return { DiagnosticLevel::Error, "E3010" };
        case DiagnosticID::CharEscapeEmptyUnicode: return { DiagnosticLevel::Error, "E3011" };
        case DiagnosticID::CharEscapeOverlongUnicode: return { DiagnosticLevel::Error, "E3012" };
        case DiagnosticID::CharEscapeInvalidUnicodeRange: return { DiagnosticLevel::Error, "E3013" };

        // String literal errors
        case DiagnosticID::StringUnterminated: return { DiagnosticLevel::Error, "E3101" };
        case DiagnosticID::StringEscapeUnknown: return { DiagnosticLevel::Error, "E3102" };
        case DiagnosticID::StringEscapeHexTooShort: return { DiagnosticLevel::Error, "E3103" };
        case DiagnosticID::StringEscapeInvalidHexDigit: return { DiagnosticLevel::Error, "E3104" };
        case DiagnosticID::StringEscapeHexOutOfRange: return { DiagnosticLevel::Error, "E3105" };
        case DiagnosticID::StringEscapeMissingUnicodeBrace: return { DiagnosticLevel::Error, "E3106" };
        case DiagnosticID::StringEscapeInvalidUnicodeDigit: return { DiagnosticLevel::Error, "E3107" };
        case DiagnosticID::StringEscapeUnterminatedUnicode: return { DiagnosticLevel::Error, "E3108" };
        case DiagnosticID::StringEscapeEmptyUnicode: return { DiagnosticLevel::Error, "E3109" };
        case DiagnosticID::StringEscapeOverlongUnicode: return { DiagnosticLevel::Error, "E3110" };
        case DiagnosticID::StringEscapeInvalidUnicodeRange: return { DiagnosticLevel::Error, "E3111" };

        case DiagnosticID::UnrecognizedSymbol: return { DiagnosticLevel::Error, "E1010" };

        default: return { DiagnosticLevel::Error, "E0000" };
    }
}
