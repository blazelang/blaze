#pragma once

#include <string>
#include <unicode/normalizer2.h>

namespace utf8 {
    /// Decodes a single UTF-8 codepoint from input[index]
    /// Returns number of bytes consumed. `cp` is filled with the decoded char32_t.
    /// If invalid, returns 0 and sets cp to 0xFFFD.
    inline size_t decodeCodepoint(const std::string_view input, size_t index, char32_t& cp) {
        if (index >= input.size()) {
            cp = 0xFFFD;
            return 0;
        }

        const uint8_t* s = reinterpret_cast<const uint8_t*>(input.data());
        int32_t offset = static_cast<int32_t>(index);
        int32_t length = static_cast<int32_t>(input.size());

        UChar32 codepoint = 0;

        int32_t startOffset = offset;
        U8_NEXT(s, offset, length, codepoint);

        if (codepoint < 0) {
            cp = 0xFFFD; // Replacement character
            return 1;    // Consume 1 byte on error
        }

        cp = static_cast<char32_t>(codepoint);
        return static_cast<size_t>(offset - startOffset);
    }

    // Encodes a single Unicode codepoint to UTF-8 string
    // Returns a std::string with the encoded result
    inline std::string encodeCodepoint(uint32_t cp) {
        if (cp > 0x10FFFF) {
            return "\xEF\xBF\xBD"; // U+FFFD replacement character
        }

        char buffer[4];
        int32_t offset = 0;

        U8_APPEND_UNSAFE(reinterpret_cast<uint8_t*>(buffer), offset, static_cast<UChar32>(cp));

        return std::string(buffer, buffer + offset);
    }

    inline std::string normalizeToNFKC(const std::string_view lexeme) {
        UErrorCode errorCode = U_ZERO_ERROR;

        // Get NFKC Normalizer2 instance
        const icu::Normalizer2* normalizer = icu::Normalizer2::getInstance(nullptr, "nfkc", UNORM2_COMPOSE, errorCode);
        if (U_FAILURE(errorCode)) {
            throw std::runtime_error("Failed to get Normalizer2 instance");
        }

        // Build UnicodeString from UTF-32 codepoints
        icu::UnicodeString ustr = icu::UnicodeString::fromUTF8(lexeme);

        // Normalize to NFKC
        icu::UnicodeString normalizedUnicodeString;
        normalizer->normalize(ustr, normalizedUnicodeString, errorCode);
        if (U_FAILURE(errorCode)) {
            throw std::runtime_error("Normalization failed");
        }

        std::string normalizedLexeme;
        normalizedUnicodeString.toUTF8String(normalizedLexeme);

        return normalizedLexeme;
    }
}
