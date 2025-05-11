#include <string>
#include <cstdint>
#include <unicode/umachine.h>
#include <unicode/utf8.h>

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
}
