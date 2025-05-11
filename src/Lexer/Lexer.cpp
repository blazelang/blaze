#include "Lexer/Lexer.hpp"

#include <cstdint>
#include <exception>
#include <optional>
#include <string>
#include <format>
#include <string_view>

#include <unicode/normalizer2.h>

#include "Lexer/Token.hpp"
#include "Utils/Utf8.hpp"

Lexer::Lexer(
    ISourceManager::FileID fileID,
    ISourceManager& sourceManager,
    IDiagnosticEngine& diagnosticEngine
) : m_fileID(fileID),
    m_sourceManager(sourceManager),
    m_diagnosticEngine(diagnosticEngine),
    m_source(sourceManager.getBuffer(fileID)),
    m_start(0),
    m_pos(0),
    m_line(1),
    m_column(1) {}

char32_t Lexer::advance() {
    char32_t cp = 0;
    size_t bytes = utf8::decodeCodepoint(m_source, m_pos, cp);
    m_pos += bytes;
    m_column += 1;
    return cp;
};

char32_t Lexer::peek() const {
    char32_t cp = 0;
    utf8::decodeCodepoint(m_source, m_pos, cp);
    return cp;
};

char32_t Lexer::lookahead(int i) const {
    char32_t cp = 0;
    size_t bytes = 0;
    size_t escapeSequenceEndPos = m_pos;
    for (int count = 0; count <= i; ++count) {
        if (escapeSequenceEndPos >= m_source.size()) {
            return U'\0'; // End of source
        }
        cp = 0;
        bytes = utf8::decodeCodepoint(m_source, escapeSequenceEndPos, cp);
        escapeSequenceEndPos += bytes;
    }
    return cp;
};

bool Lexer::match(const char32_t codepoint) {
    if (isEnd() || codepoint != peek()) {
        return false;
    }
    advance();
    return true;
};

SourcePosition Lexer::currentPosition() const {
    return {
        .fileName = m_sourceManager.getPath(m_fileID),
        .position = std::pair(m_line, m_column),
    };
}

void Lexer::reportError(const std::string& message) {
    m_diagnosticEngine.report({
        .phrase = DiagnosticPhrase::Lexer,
        .lvl = DiagnosticLevel::Error,
        .position = currentPosition(),
        .message = message,
    });
}

void Lexer::reportWarning(const std::string& message) {
    m_diagnosticEngine.report({
        .phrase = DiagnosticPhrase::Lexer,
        .lvl = DiagnosticLevel::Warning,
        .position = currentPosition(),
        .message = message,
    });
}

std::string_view Lexer::getLexeme() const {
    return m_source.substr(m_start, m_pos - m_start);
}

void Lexer::addToken(TokenKind kind, std::optional<std::string> lexeme) {
    size_t column = m_column - (m_pos - m_start);
    m_tokens.push_back({
        .kind = kind,
        .position = std::pair(m_line, column),
        .lexeme = lexeme.value_or(std::string(getLexeme()))
    });
}

std::string Lexer::normalizeToNFKC(const std::string_view lexeme) {
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

bool Lexer::isEnd() const {
    return m_pos >= m_source.length();
}

bool Lexer::isWhitespace(char32_t codepoint) const {
    return codepoint == U' ' || codepoint == U'\t' || codepoint == U'\n' || codepoint == U'\r' || codepoint == U'\v' || codepoint == U'\f';
}

bool Lexer::isHexDigit(char32_t codepoint) const {
    return (codepoint >= U'0' && codepoint <= U'9') || (codepoint >= U'a' && codepoint <= U'f') || (codepoint >= U'A' && codepoint <= U'F');
}

bool Lexer::isOctalDigit(char32_t codepoint) const {
    return codepoint >= U'0' && codepoint <= U'7';
}

bool Lexer::isBinaryDigit(char32_t codepoint) const {
    return codepoint == U'0' || codepoint == U'1';
}

bool Lexer::isDigit(Lexer::NumericBase base, char32_t codepoint) const {
    if (base == NumericBase::Binary) {
        return isBinaryDigit(codepoint);
    } else if (base == NumericBase::Octal) {
        return isOctalDigit(codepoint);
    } else if (base == NumericBase::HexaDecimal) {
        return isHexDigit(codepoint);
    } else {
        return isDecimalDigit(codepoint);
    }
}

bool Lexer::isDecimalDigit(char32_t codepoint) const {
    return (codepoint >= U'0' && codepoint <= U'9');
}

bool Lexer::isNumberStart(char32_t codepoint) const {
    return isDecimalDigit(codepoint) || (codepoint == U'.' && isDecimalDigit(peek())) || (codepoint == U'_' && isDecimalDigit(peek()));
}

bool Lexer::isAlpha(char32_t codepoint) const {
    return (codepoint >= 'a' && codepoint <= 'z') || (codepoint >= 'A' && codepoint <= 'Z') || codepoint == '_';
}

bool Lexer::isAlphaNum(char32_t codepoint) const {
    return isAlpha(codepoint) || isDecimalDigit(codepoint);
}

bool Lexer::isIdentifierStart(char32_t cp) {
    return u_hasBinaryProperty(cp, UCHAR_XID_START);
};

bool Lexer::isIdentifierContinue(char32_t cp) {
    return u_hasBinaryProperty(cp, UCHAR_XID_CONTINUE);
};

bool Lexer::matchEscapeSequence(EscapeSequenceErrorKind& errorKind) {
    switch (peek()) {
        case '\\':
        case '\'':
        case '\"':
        case 'n' :
        case 'r' :
        case 't' :
        case 'b' :
        case 'f' :
        case 'v' :
        case '0' : {
            advance();
            return true;
        }

        // Hex escape: \xNN
        case 'x' : {
            advance(); // consume 'x`

            std::string hexString;
            hexString.reserve(2);

            if (peek() == U'\'' || peek() == U'\"') {
                errorKind = EscapeSequenceErrorKind::NumericEscapeTooShort;
                return false;
            }

            if (!isHexDigit(peek())) {
                errorKind = EscapeSequenceErrorKind::InvalidNumericEscape;
                return false;
            }

            hexString += static_cast<char>(advance()); // Consume first hex digit

            if (peek() == U'\'' || peek() == U'\"') {
                errorKind = EscapeSequenceErrorKind::NumericEscapeTooShort;
                return false;
            }

            if (!isHexDigit(peek())) {
                errorKind = EscapeSequenceErrorKind::InvalidNumericEscape;
                return false;
            }

            hexString += static_cast<char>(advance()); // Consume second hex digit

            // Check if hex value is in range of 00 to 7f
            uint32_t hexValue;
            try {
               hexValue = std::stoul(hexString, nullptr, 16);
            } catch (const std::exception&) {
                errorKind = EscapeSequenceErrorKind::HexOutOfRange;
                return false;
            }

            if (hexValue > 0x7F) { // Check for 00-7F range
                errorKind = EscapeSequenceErrorKind::HexOutOfRange;
                return false;
            }

            return true;
        }

        // Unicode escape: \u{NNNNNN}
        case 'u' : {
            advance(); // Consume 'u'

            if (!match(U'{')) {
                errorKind = EscapeSequenceErrorKind::MalformedUnicodeSequence;
                return false;
            }

            std::string hexString;
            hexString.reserve(6);
            int digitCount = 0;

            while (!isEnd() && peek() != U'\'' && peek() != U'\"' && peek() != U'}') {
                if (!isHexDigit(peek())) {
                    errorKind = EscapeSequenceErrorKind::InvalidUnicodeChar;
                    return false;
                }
                hexString += static_cast<char>(advance());
                digitCount += 1;
            }

            if (!match(U'}')) {
                errorKind = EscapeSequenceErrorKind::UnterminatedUnicode;
                return false;
            }

            if (digitCount == 0) {
                errorKind = EscapeSequenceErrorKind::EmptyUnicodeEscape;
                return false;
            }

            if (digitCount > 6) {
                errorKind = EscapeSequenceErrorKind::OverlongUnicodeChar;
                return false;
            }

            uint32_t hexValue;
            try {
                hexValue = std::stoul(hexString, nullptr, 16);
            } catch (const std::out_of_range&) {
                // Max FFFFFF is 16,777,215, fits in unsigned long. Unlikely.
                errorKind = EscapeSequenceErrorKind::UnicodeOutOfRange;
                return false;
            }

            if (hexValue > 0x10FFFF || (hexValue >= 0xD800 && hexValue <= 0xDFFF)) {
                errorKind = EscapeSequenceErrorKind::UnicodeOutOfRange;
                return false;
            }

            return true;
        }
        default: {
            errorKind = EscapeSequenceErrorKind::UnknownEscape;
            return false;
        }
    }
}

void Lexer::skipWhitespace(char32_t codepoint) {
    if (codepoint == U'\n') {
        m_line += 1;
        m_column = 1;
    }
}

void Lexer::lexLineComment() {
    std::optional<TokenKind> token;

    if (match(U'/')) {
        token = TOK_DOC_COMMENT_LINE_OUTER;
    } else if (match(U'!')) {
        token = TOK_DOC_COMMENT_LINE_INNER;
    }

    while (!isEnd() && peek() != U'\n') {
        advance();
    }

    if (token.has_value()) {
        return addToken(token.value());
    }
}

void Lexer::lexBlockComment() {
    int depth = 1;
    std::optional<TokenKind> token;

    if (match(U'*')) {
        token = TOK_DOC_COMMENT_BLOCK_OUTER;
    } else if (match(U'!')) {
        token = TOK_DOC_COMMENT_BLOCK_INNER;
    }

    while (!isEnd()) {
        if (peek() == U'/' && lookahead() == U'*') {
            // Nested opening block comment
            advance();
            advance();
            depth += 1;
        } else if (peek() == U'*' && lookahead() == U'/') {
            // Closing block comment
            advance();
            advance();
            depth -= 1;
        }

        // Break if depth is zero, found closing delimiter for block comment
        if (depth == 0) {
            break;
        }

        if (peek() == U'\n') {
            m_line += 1;
            m_column = 1;
        }

        advance();
    }

    // Handle the case where the comment is unterminated (depth > 0)
    if (depth > 0) {
        reportError(std::format("Unterminated block comment"));
        return addToken(TOK_ERROR);
    }

    if (token.has_value()) {
        addToken(token.value());
    }
}

void Lexer::lexKeywordOrIdentifier() {
    // Keep advancing codepoints as long as they are valid continuation characters
    while (!isEnd() && isIdentifierContinue(peek())) {
        advance();
    }

    std::string normalizedUTF8Lexeme = normalizeToNFKC(getLexeme());

    // Look up the normalized lexeme in the keyword map
    auto keywordIt = g_keywordMap.find(normalizedUTF8Lexeme);

    // If it's a keyword, add the keyword token, otherwise add it as an identifier
    if (keywordIt != g_keywordMap.end()) {
        addToken(keywordIt->second);
    } else {
        addToken(TOK_IDENTIFIER, normalizedUTF8Lexeme);
    }
}

void Lexer::lexNumberLiteral(char32_t codepoint) {
    // Default number base is decimal
    NumericBase base = Decimal;
    bool isFloat = false;

    // Validation flags
    bool hasEmptyDigit = false;
    bool hasInValidDigit = false;
    bool hasEmptyExponent = false;
    bool hasLeadingDecimalDot = false;
    bool hasMultipleDecimalDot = false;
    bool hasConsecutiveUnderscore = false;
    bool hasLeadingUnderscore = false;
    bool hasTailingUnderscore = false;
    bool hasUnderscoreBeforeBasePrefix = false;
    bool hasUnderscoreAfterBasePrefix = false;
    bool hasUnderscoreBeforeDecimalPoint = false;

    std::string invalidSuffix;

    // Case when the literal starts with '.' (e.g., ".123") which is invalid
    if (codepoint == U'.') {
        hasLeadingDecimalDot = true;
        advance();
    }

    // Check for invalid underscore at start after digit (e.g., "_0123" or "_123")
    if (codepoint == U'_') {
        hasLeadingUnderscore = true;
        advance();
    }

    // Check for invalid underscore between '0' and base prefix (e.g., "0_xFF")
    if (codepoint == U'0' && peek() == U'_') {
        hasTailingUnderscore = true;
        advance();
    }

    // parse base prefix
    if (match(U'x') || match(U'X')) {
        base = HexaDecimal;
    } else if (match(U'b') || match(U'B')) {
        base = Binary;
    } else if (match(U'o') || match(U'O')) {
        base = Octal;
    }

    // Check for invalid underscore if base is not decimal and has tailing underscore (eg. "0_x" or "0_b")
    if (base != Decimal && hasTailingUnderscore) {
        hasUnderscoreBeforeBasePrefix = true;
        hasTailingUnderscore= false;
    }

    // Check for invalid underscore after base prefix (e.g., "0x_FF")
    if (base != Decimal && peek() == U'_') {
        hasUnderscoreAfterBasePrefix = true;
        advance();
    }

    // Validate first digit after base prefix
    if (base != Decimal && !isDigit(base, peek())) {
        if (isAlphaNum(peek())) {
            hasInValidDigit = true; // invalid character
        } else {
            hasEmptyDigit = true; // no valid digit present
        }
    }

    // Parse digits and underscores in numeric literal
    while(isDigit(base, peek()) || peek() == U'_') {
        char32_t current = peek();
        char32_t next = lookahead();

        // Consecutive underscores (e.g., "123__456") are not allowed
        if (current == U'_') {
            if (next == U'_') {
                hasConsecutiveUnderscore = true;
            }
            hasTailingUnderscore = true;
        } else {
            hasTailingUnderscore = false;
        }

        advance();
    }

    // Decimal point support only in base 10 and decimal number after ('.')
    if (base == Decimal && peek() == U'.' && isDecimalDigit(lookahead())) {
        // check for underscore before decimal point
        if (hasTailingUnderscore) {
            hasTailingUnderscore = false;
            hasUnderscoreBeforeDecimalPoint = true;
        } else {
            isFloat = true;
        }
        advance(); // consume ('.')
    }

    // Parse decimal fraction part
    while (isDecimalDigit(peek()) || peek() == U'_') {
        char32_t current = peek();
        char32_t next = lookahead();

        // Check for consecutive and last underscore
        if (peek() == U'_') {
            if (next == U'_') {
                hasConsecutiveUnderscore = true;
            }
            hasTailingUnderscore = true;
        } else {
            hasTailingUnderscore = false;
        }

        advance();
    }

    // Check for multiple decimal dot (1.2.3.4)
    if (isFloat && peek() == U'.' && isDecimalDigit(lookahead())) {
        hasMultipleDecimalDot = true;
        advance();
        while (isDecimalDigit(peek()) || peek() == U'.') {
            advance();
        }
    }

    // Handle exponent (e.g., "1.0e+10")
    if (base == Decimal && (match(U'e') || match(U'E'))) {
        isFloat = true;

        if (peek() == '+' || peek() == '-') {
            advance(); // consume optional sign '+' or '-'
        }

        // check for atleast one valid decimal digit for exponent is found
        if (!isDecimalDigit(peek())) {
            hasEmptyExponent = true; // e.g., "1.0e"
        }

        while (isDecimalDigit(peek())) {
            advance();
        }
    }

    // Handle valid integer suffixes like i32, u64, etc.
    if (!isFloat && (peek() == U'i' || peek() == U'I' || peek() == U'u' || peek() == U'U')) {
        if (lookahead() == U'8') {
            advance(); advance();
        } else if (lookahead() == U'1') {
            if (lookahead(2) == U'6') {
                advance(); advance(); advance();
            } else if (lookahead(2) == U'2' && lookahead(3) == U'8') {
                advance(); advance(); advance(); advance();
            }
        } else if (lookahead() == U'3' && lookahead(2) == U'2') {
            advance(); advance(); advance();
        } else if (lookahead() == U'6' && lookahead(2) == U'4') {
            advance(); advance(); advance();
        }
    }

    // Handle float suffixes like f32, f64
    if (peek() == U'f' || peek() == U'F') {
        isFloat = true;
        if (lookahead() == U'3' && lookahead(2) == U'2') {
            advance(); advance(); advance();
        } else if (lookahead() == U'6' && lookahead(2) == U'4') {
            advance(); advance(); advance();
        }
    }

    // Catch any remaining invalid suffix
    while (isAlphaNum(peek())) {
        invalidSuffix += advance();
    }

    // Final validations and error reports

    if (invalidSuffix.size() > 0) {
        reportError(std::format("invalid suffix `{}` for {} literal", invalidSuffix, isFloat ? "float" : "number"));
        if (isFloat) {
            reportWarning("valid suffixes are `f32` and `f64`");
        } else {
            reportWarning("valid suffix must be one of the numeric types (`i32` ,`u32`, `isize`, `usize` etc.)");
        }
        return addToken(TOK_ERROR);
    }

    if (hasEmptyDigit) {
        reportError("no valid digit found for number");
        return addToken(TOK_ERROR);
    }

    if (hasLeadingDecimalDot) {
        reportError("float literals must have an integer part before decimal dot");
        return addToken(TOK_ERROR);
    }

    if (hasMultipleDecimalDot) {
        reportError(std::format("multiple decimal dots are not allowed in {} literal", isFloat ? "float" : "number"));
        return addToken(TOK_ERROR);
    }

    if (hasInValidDigit) {
        reportError(std::format("invalid digit for a base {} literal", static_cast<int>(base)));
        return addToken(TOK_ERROR);
    }

    if (hasEmptyExponent) {
        reportError("expected at least one digit in exponent");
        return addToken(TOK_ERROR);
    }

    if (hasConsecutiveUnderscore) {
        reportError(std::format("multiple consecutive underscores are not allowed in {} literal", isFloat ? "float" : "number"));
        return addToken(TOK_ERROR);
    }

    if (hasLeadingUnderscore) {
        reportError(std::format("leading underscore are not allowed in {} literal", isFloat ? "float" : "number"));
        return addToken(TOK_ERROR);
    }

    if (hasTailingUnderscore) {
        reportError(std::format("tailing underscore are not allowed in {} literal", isFloat ? "float" : "number"));
        return addToken(TOK_ERROR);
    }

    if (hasUnderscoreBeforeBasePrefix) {
        reportError("underscores are not allowed before base suffix in numeric literal");
        return addToken(TOK_ERROR);
    }

    if (hasUnderscoreAfterBasePrefix) {
        reportError("underscores are not allowed after base suffix in numeric literal");
        return addToken(TOK_ERROR);
    }

    if (hasUnderscoreBeforeDecimalPoint) {
        reportError("underscores are not allowed before decimal point in numeric literal");
        return addToken(TOK_ERROR);
    }

    // If all is well, return appropriate token
    return addToken(isFloat ? TOK_FLOAT_LITERAL : TOK_INTEGER_LITERAL);
}

void Lexer::lexCharLiteral() {
    bool hasMultiCodepoint = false;
    bool hasUnterminatedQuote = false;
    bool hasEscapeSequenceError = false;
    EscapeSequenceErrorKind escapeSequenceErrorKind;

    if (match('\'')) {
        reportError("empty char literal");
        return addToken(TOK_ERROR);
    }

    if (match(U'\\')) {
        hasEscapeSequenceError = !matchEscapeSequence(escapeSequenceErrorKind);
    } else {
        advance(); // consume the first codepoint for both char & string literal
    }

    int escapeSequenceEndPos = m_pos;

    // Consume all the codepoint until ending single quote
    if (peek() != U'\'') {
        while(!isEnd() && peek() != U'\n' && peek() != U'\'') {
            advance();
        }
        if (match(U'\'')) {
            hasMultiCodepoint = true;
        } else {
            hasUnterminatedQuote = true;
        }
    } else {
        advance(); // consume the closing quotes
    }

    if (hasUnterminatedQuote) {
        reportError("unterminated character literal");
        return addToken(TOK_ERROR);
    }

    if (hasEscapeSequenceError) {
        switch (escapeSequenceErrorKind) {
            case EscapeSequenceErrorKind::UnknownEscape:
                reportError(std::format("unknown character escape: `{}`", m_source.at(escapeSequenceEndPos)));
                break;
            case EscapeSequenceErrorKind::NumericEscapeTooShort:
                reportError("numeric character escape is too short");
                break;
            case EscapeSequenceErrorKind::InvalidNumericEscape:
                reportError(std::format("invalid character in numeric character escape: `{}`", m_source.at(escapeSequenceEndPos)));
                break;
            case EscapeSequenceErrorKind::HexOutOfRange:
                reportError("out of range hex escape");
                break;
            case EscapeSequenceErrorKind::MalformedUnicodeSequence:
                reportError("incorrect unicode escape sequence");
                break;
            case EscapeSequenceErrorKind::EmptyUnicodeEscape:
                reportError("empty unicode escape");
                break;
            case EscapeSequenceErrorKind::InvalidUnicodeChar:
                reportError(std::format("invalid character in unicode escape: `{}`", m_source.at(escapeSequenceEndPos)));
                break;
            case EscapeSequenceErrorKind::UnterminatedUnicode:
                reportError("unterminated unicode esscape");
                break;
            case EscapeSequenceErrorKind::OverlongUnicodeChar:
                reportError("overlong unicode escape");
                break;
            case EscapeSequenceErrorKind::UnicodeOutOfRange:
                reportError("invalid character escape");
                break;
        }
        return addToken(TOK_ERROR);
    }

    if (hasMultiCodepoint) {
        reportError("character literal may only contain one codepoint");
        return addToken(TOK_ERROR);
    }

    return addToken(TOK_CHAR);
}

void Lexer::lexStringLiteral() {
    bool hasUnterminatedQuote = false;
    std::vector<std::pair<EscapeSequenceErrorKind, size_t>> escapeSequenceErrors;

    while(!isEnd() && peek() != U'\n' && peek() != '\"') {
        if (match('\\')) {
            EscapeSequenceErrorKind escapeSequenceErrorKind;
            bool isValidEscapeSequence = matchEscapeSequence(escapeSequenceErrorKind);
            if (!isValidEscapeSequence) {
                escapeSequenceErrors.push_back(std::pair(escapeSequenceErrorKind, m_pos));
            }
        } else {
            advance();
        }
    }

    if (!match(U'\"')) {
        hasUnterminatedQuote = true;
    }

    if (escapeSequenceErrors.size() > 0) {
        for (auto& escapeSequenceErrors : escapeSequenceErrors) {
            size_t escapeSequenceEndPos = escapeSequenceErrors.second;
            switch (escapeSequenceErrors.first) {
                case EscapeSequenceErrorKind::UnknownEscape:
                    reportError(std::format("unknown character escape: `{}`", m_source.at(escapeSequenceEndPos)));
                    break;
                case EscapeSequenceErrorKind::NumericEscapeTooShort:
                    reportError("numeric character escape is too short");
                    break;
                case EscapeSequenceErrorKind::InvalidNumericEscape:
                    reportError(std::format("invalid character in numeric character escape: `{}`", m_source.at(escapeSequenceEndPos)));
                    break;
                case EscapeSequenceErrorKind::HexOutOfRange:
                    reportError("out of range hex escape");
                    break;
                case EscapeSequenceErrorKind::MalformedUnicodeSequence:
                    reportError("incorrect unicode escape sequence");
                    break;
                case EscapeSequenceErrorKind::EmptyUnicodeEscape:
                    reportError("empty unicode escape");
                    break;
                case EscapeSequenceErrorKind::InvalidUnicodeChar:
                    reportError(std::format("invalid character in unicode escape: `{}`", m_source.at(escapeSequenceEndPos)));
                    break;
                case EscapeSequenceErrorKind::UnterminatedUnicode:
                    reportError("unterminated unicode esscape");
                    break;
                case EscapeSequenceErrorKind::OverlongUnicodeChar:
                    reportError("overlong unicode escape");
                    break;
                case EscapeSequenceErrorKind::UnicodeOutOfRange:
                    reportError("invalid character escape");
                    break;
            }
        }
        return addToken(TOK_ERROR);
    }

    if (hasUnterminatedQuote) {
        reportError("Unterminated string literal");
        return addToken(TOK_ERROR);
    }

    return addToken(TOK_STRING);
}

void Lexer::lexSymbol(char32_t cp) {
    std::string symbol = utf8::encodeCodepoint(cp);

    auto symbolIt = g_symbolMap.find(symbol);
    if (symbolIt == g_symbolMap.end()) {
        reportError(std::format("Unrecognized symbol '{}' (U+{:04X})", symbol, static_cast<uint32_t>(cp)));
        return addToken(TOK_ERROR);
    }

    // Try to extend the symbol as long as it's exist
    while (!isEnd()) {
        // Check if the extended symbol exist
        std::string nextSymbol = utf8::encodeCodepoint(peek());
        auto extendedSymbolIt = g_symbolMap.find(symbol + nextSymbol);
        if (extendedSymbolIt != g_symbolMap.end()) {
            symbol += advance();
            symbolIt = extendedSymbolIt;
        } else {
            break;
        }
    }

    // Add the token corresponding to the final symbol
    return addToken(symbolIt->second);
}

std::vector<Token>& Lexer::tokenize() {
    while (!isEnd()) {
        m_start = m_pos;
        const char32_t cp = advance();
        if (isWhitespace(cp)) {
            skipWhitespace(cp);
        } else if (cp == U'/' && match(U'/')) {
            lexLineComment();
        } else if (cp == U'/' && match(U'*')) {
            lexBlockComment();
        } else if (isIdentifierStart(cp)) {
            lexKeywordOrIdentifier();
        } else if (isNumberStart(cp)) {
            lexNumberLiteral(cp);
        } else if (cp == U'\'') {
            lexCharLiteral();
        } else if (cp == U'\"') {
            lexStringLiteral();
        } else {
            lexSymbol(cp);
        }
    }

    m_start = m_pos;
    addToken(TOK_EOF);

    // Reset values
    m_start = 0; m_pos = 0; m_line = 1; m_column = 1;

    return m_tokens;
}
