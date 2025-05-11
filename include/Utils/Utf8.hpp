#include <string>
#include <cstdint>
#include <stdexcept>

namespace utf8 {
    // Exception thrown on invalid UTF-8 sequence
    class invalid_encoding : public std::runtime_error {
    public:
        explicit invalid_encoding(const std::string& msg) : std::runtime_error(msg) {}
    };

    // Define the Unicode Replacement Character (U+FFFD)
    constexpr char32_t REPLACEMENT_CHARACTER = 0xFFFD;

    inline size_t decodeCodepoint(const std::string_view input, size_t index, char32_t& cp) {
        if (index >= input.length()) {
            cp = REPLACEMENT_CHARACTER;
            return 0; // Index is out of bounds, no bytes consumed.
        }

        // It's safer to work with unsigned char for bitwise operations on byte values.
        const unsigned char b1 = static_cast<unsigned char>(input[index]);

        // 1-byte sequence (0xxxxxxx) - ASCII characters
        if (b1 < 0x80) { // 0x00 - 0x7F
            cp = static_cast<char32_t>(b1);
            return 1;
        }

        // 2-byte sequence (110xxxxx 10xxxxxx)
        if ((b1 & 0xE0) == 0xC0) { // 0xC0 - 0xDF
            if (index + 1 >= input.length()) { // Not enough bytes
                cp = REPLACEMENT_CHARACTER;
                return 1; // Consumed the invalid starting byte
            }
            const unsigned char b2 = static_cast<unsigned char>(input[index + 1]);
            if ((b2 & 0xC0) != 0x80) { // b2 is not a continuation byte (10xxxxxx)
                cp = REPLACEMENT_CHARACTER;
                return 1;
            }
            char32_t decoded_cp = ((static_cast<char32_t>(b1 & 0x1F)) << 6) |
                                   (static_cast<char32_t>(b2 & 0x3F));

            // Check for overlong encoding (codepoint must be >= 0x80 for 2-byte sequence)
            // Also, b1 must not be 0xC0 or 0xC1 which are always overlong.
            // (0xC0, 0xC1 would result in decoded_cp < 0x80)
            if (decoded_cp < 0x80) {
                cp = REPLACEMENT_CHARACTER;
                return 1;
            }
            cp = decoded_cp;
            return 2;
        }

        // 3-byte sequence (1110xxxx 10xxxxxx 10xxxxxx)
        if ((b1 & 0xF0) == 0xE0) { // 0xE0 - 0xEF
            if (index + 2 >= input.length()) { // Not enough bytes
                cp = REPLACEMENT_CHARACTER;
                return 1;
            }
            const unsigned char b2 = static_cast<unsigned char>(input[index + 1]);
            const unsigned char b3 = static_cast<unsigned char>(input[index + 2]);
            if ((b2 & 0xC0) != 0x80 || (b3 & 0xC0) != 0x80) { // Check continuation bytes
                cp = REPLACEMENT_CHARACTER;
                return 1;
            }
            char32_t decoded_cp = ((static_cast<char32_t>(b1 & 0x0F)) << 12) |
                                  ((static_cast<char32_t>(b2 & 0x3F)) << 6)  |
                                   (static_cast<char32_t>(b3 & 0x3F));

            // Check for overlong encoding (codepoint must be >= 0x800 for 3-byte)
            // e.g. U+0000 encoded as E0 80 80
            // This also catches b1 == 0xE0 && b2 < 0xA0 (RFC 3629).
            if (decoded_cp < 0x800) {
                cp = REPLACEMENT_CHARACTER;
                return 1;
            }
            // Check for UTF-16 surrogate pairs (U+D800 to U+DFFF), which are invalid in UTF-8
            if (decoded_cp >= 0xD800 && decoded_cp <= 0xDFFF) {
                cp = REPLACEMENT_CHARACTER;
                return 1;
            }
            cp = decoded_cp;
            return 3;
        }

        // 4-byte sequence (11110xxx 10xxxxxx 10xxxxxx 10xxxxxx)
        if ((b1 & 0xF8) == 0xF0) { // 0xF0 - 0xF7 (Standard UTF-8 only goes up to 0xF4)
            if (index + 3 >= input.length()) { // Not enough bytes
                cp = REPLACEMENT_CHARACTER;
                return 1;
            }
            const unsigned char b2 = static_cast<unsigned char>(input[index + 1]);
            const unsigned char b3 = static_cast<unsigned char>(input[index + 2]);
            const unsigned char b4 = static_cast<unsigned char>(input[index + 3]);
            if ((b2 & 0xC0) != 0x80 || (b3 & 0xC0) != 0x80 || (b4 & 0xC0) != 0x80) { // Check continuation
                cp = REPLACEMENT_CHARACTER;
                return 1;
            }
            char32_t decoded_cp = ((static_cast<char32_t>(b1 & 0x07)) << 18) |
                                  ((static_cast<char32_t>(b2 & 0x3F)) << 12) |
                                  ((static_cast<char32_t>(b3 & 0x3F)) << 6)  |
                                   (static_cast<char32_t>(b4 & 0x3F));

            // Check for overlong encoding (codepoint must be >= 0x10000 for 4-byte)
            // This also catches b1 == 0xF0 && b2 < 0x90 (RFC 3629).
            if (decoded_cp < 0x10000) {
                cp = REPLACEMENT_CHARACTER;
                return 1;
            }
            // Check if codepoint is outside the valid Unicode range (max U+10FFFF)
            // (b1 values 0xF5 and above would lead to this).
            if (decoded_cp > 0x10FFFF) {
                cp = REPLACEMENT_CHARACTER;
                return 1;
            }
            cp = decoded_cp;
            return 4;
        }

        // If none of the above patterns matched, it's an invalid UTF-8 start byte
        // or a continuation byte appearing where a start byte is expected.
        cp = REPLACEMENT_CHARACTER;
        return 1; // Consume the invalid byte.
    }

    inline std::string encodeCodepoint(uint32_t cp) {
        std::string result;
        if (cp <= 0x7F) {
            result += static_cast<char>(cp);
        } else if (cp <= 0x7FF) {
            result += static_cast<char>(0xC0 | (cp >> 6));
            result += static_cast<char>(0x80 | (cp & 0x3F));
        } else if (cp <= 0xFFFF) {
            if (cp >= 0xD800 && cp <= 0xDFFF) {
                throw invalid_encoding("Invalid code point: surrogate");
            }
            result += static_cast<char>(0xE0 | (cp >> 12));
            result += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
            result += static_cast<char>(0x80 | (cp & 0x3F));
        } else if (cp <= 0x10FFFF) {
            result += static_cast<char>(0xF0 | (cp >> 18));
            result += static_cast<char>(0x80 | ((cp >> 12) & 0x3F));
            result += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
            result += static_cast<char>(0x80 | (cp & 0x3F));
        } else {
            throw invalid_encoding("Code point out of range");
        }
        return result;
    }
}
