#include "Lexer/Lexer.hpp"

#include <optional>
#include <string>
#include <string_view>

#include "Diagnostics/DiagnosticID.hpp"
#include "Diagnostics/DiagnosticBuilder.hpp"
#include "Utils/Utf8.hpp"

Lexer::Lexer(ISourceManager::FileID fileID, ISourceManager& sourceManager, IDiagnosticEngine& diagnosticEngine)
:   m_fileID(fileID),
    m_sourceManager(sourceManager),
    m_diagnosticEngine(diagnosticEngine),
    m_source(sourceManager.getBuffer(fileID)) {}

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
    size_t pos = m_pos;
    for (int count = 0; count <= i; ++count) {
        if (pos >= m_source.size()) {
            return U'\0'; // End of source
        }
        cp = 0;
        bytes = utf8::decodeCodepoint(m_source, pos, cp);
        pos += bytes;
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

std::string_view Lexer::getLexeme() const {
    return m_source.substr(m_start, m_pos - m_start);
}

void Lexer::addToken(TokenKind kind, std::optional<std::string> lexeme) {
    m_tokens.push_back({
        .kind = kind,
        .position = std::pair(m_startLine, m_startColumn),
        .lexeme = lexeme.value_or(std::string(getLexeme()))
    });
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
        m_diagnosticEngine.report(DiagnosticID::UnterminatedBlockComment)
            .message("Unterminated {} comment", token.has_value() ? "document" : "block")
            .span({ m_fileID, m_startLine, m_startColumn })
            .build();
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

    std::string normalizedUTF8Lexeme = utf8::normalizeToNFKC(getLexeme());

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
    bool hasLeadingDot = false;
    bool hasMultipleDot = false;
    bool hasConsecutiveUnderscore = false;
    bool hasLeadingUnderscore = false;
    bool hasTailingUnderscore = false;
    bool hasUnderscoreBeforeBasePrefix = false;
    bool hasUnderscoreAfterBasePrefix = false;
    bool hasUnderscoreBeforeDot = false;

    std::string invalidSuffix;

    size_t invalidLine = m_startLine;
    size_t invalidColumn = m_startColumn;

    // Case when the literal starts with '.' (e.g., ".123") which is invalid
    if (codepoint == U'.') {
        hasLeadingDot = true;
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
            hasUnderscoreBeforeDot = true;
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
        hasMultipleDot = true;
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
    if (!hasInValidDigit && isAlphaNum(peek())) {
        invalidLine = m_line;
        invalidColumn = m_column;
        while (isAlphaNum(peek())) {
            invalidSuffix += advance();
        }
    }

    // Final validations and error reports

    if (invalidSuffix.size() > 0) {
        m_diagnosticEngine.report(DiagnosticID::NumberLiteralInvalidSuffix)
            .message("Invalid suffix `{}` on number literal", invalidSuffix)
            .span({ m_fileID, invalidLine, invalidColumn })
            .build();
        return addToken(TOK_ERROR);
    }

    if (hasEmptyDigit) {
        m_diagnosticEngine.report(DiagnosticID::NumberLiteralEmptyDigits)
            .message("Numeric literal contains no digits")
            .span({ m_fileID, invalidLine, invalidColumn })
            .build();
        return addToken(TOK_ERROR);
    }

    if (hasLeadingDot) {
        m_diagnosticEngine.report(DiagnosticID::NumberLiteralLeadingDot)
            .message("Floating-point literals must include digits before the decimal point")
            .span({ m_fileID, invalidLine, invalidColumn })
            .build();
        return addToken(TOK_ERROR);
    }

    if (hasMultipleDot) {
        m_diagnosticEngine.report(DiagnosticID::NumberLiteralMultipleDots)
            .message("Numeric literal contains multiple decimal points")
            .span({ m_fileID, invalidLine, invalidColumn })
            .build();
        return addToken(TOK_ERROR);
    }

    if (hasInValidDigit) {
        m_diagnosticEngine.report(DiagnosticID::NumberLiteralInvalidDigit)
            .message("Numeric literal contains invalid digit(s)")
            .span({ m_fileID, invalidLine, invalidColumn })
            .build();
        return addToken(TOK_ERROR);
    }

    if (hasEmptyExponent) {
        m_diagnosticEngine.report(DiagnosticID::NumberLiteralEmptyExponent)
            .message("Exponent in numeric literal must contain at least one digit")
            .span({ m_fileID, invalidLine, invalidColumn })
            .build();
        return addToken(TOK_ERROR);
    }

    if (hasConsecutiveUnderscore) {
        m_diagnosticEngine.report(DiagnosticID::NumberLiteralConsecutiveUnderscore)
            .message("Consecutive underscores are not permitted within numeric literals")
            .span({ m_fileID, invalidLine, invalidColumn })
            .build();
        return addToken(TOK_ERROR);
    }

    if (hasLeadingUnderscore) {
        m_diagnosticEngine.report(DiagnosticID::NumberLiteralLeadingUnderscore)
            .message("Numeric literals cannot begin with an underscore")
            .span({ m_fileID, invalidLine, invalidColumn })
            .build();
        return addToken(TOK_ERROR);
    }

    if (hasTailingUnderscore) {
        m_diagnosticEngine.report(DiagnosticID::NumberLiteralTrailingUnderscore)
            .message("Numeric literals cannot end with an underscore")
            .span({ m_fileID, invalidLine, invalidColumn })
            .build();
        return addToken(TOK_ERROR);
    }

    if (hasUnderscoreBeforeBasePrefix) {
        m_diagnosticEngine.report(DiagnosticID::NumberLiteralUnderscoreBeforePrefix)
            .message("Underscores are not allowed before the base prefix in numeric literals")
            .span({ m_fileID, invalidLine, invalidColumn })
            .build();
        return addToken(TOK_ERROR);
    }

    if (hasUnderscoreAfterBasePrefix) {
        m_diagnosticEngine.report(DiagnosticID::NumberLiteralUnderscoreAfterPrefix)
            .message("Underscores are not allowed immediately after the base prefix in numeric literals")
            .span({ m_fileID, invalidLine, invalidColumn })
            .build();
        return addToken(TOK_ERROR);
    }

    if (hasUnderscoreBeforeDot) {
        m_diagnosticEngine.report(DiagnosticID::NumberLiteralUnderscoreBeforeDot)
            .message("Underscores are not allowed immediately before the decimal point in numeric literals")
            .span({ m_fileID, invalidLine, invalidColumn })
            .build();
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

    size_t invalidLine = m_startLine;
    size_t invalidColumn = m_startColumn;

    if (match('\'')) {
        m_diagnosticEngine.report(DiagnosticID::EmptyChar)
            .message("Character literal cannot be empty")
            .span({ m_fileID, invalidLine, invalidColumn })
            .build();
        return addToken(TOK_ERROR);
    }

    if (match(U'\\')) {
        hasEscapeSequenceError = !matchEscapeSequence(escapeSequenceErrorKind);
    } else {
        advance(); // consume the first codepoint for both char & string literal
    }

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
        m_diagnosticEngine.report(DiagnosticID::UnterminatedChar)
            .message("Unterminated character literal")
            .span({ m_fileID, invalidLine, invalidColumn })
            .build();
        return addToken(TOK_ERROR);
    }

    if (hasEscapeSequenceError) {
        // TODO: properly handle escape error kind for char literal
        return addToken(TOK_ERROR);
    }

    if (hasMultiCodepoint) {
        m_diagnosticEngine.report(DiagnosticID::MultiChar)
            .message("Character literal must contain only one character")
            .span({ m_fileID, invalidLine, invalidColumn })
            .build();
        return addToken(TOK_ERROR);
    }

    return addToken(TOK_CHAR);
}

void Lexer::lexStringLiteral() {
    bool hasUnterminatedQuote = false;
    bool hasEscapeSequenceError = false;

    size_t invalidLine = m_startLine;
    size_t invalidColumn = m_startColumn;

    while(!isEnd() && peek() != U'\n' && peek() != '\"') {
        if (match('\\')) {
            EscapeSequenceErrorKind escapeSequenceErrorKind;
            hasEscapeSequenceError = matchEscapeSequence(escapeSequenceErrorKind);
            // TODO: properly handle escape error kind for string literal
        } else {
            advance();
        }
    }

    if (!match(U'\"')) {
        hasUnterminatedQuote = true;
    }

    if (hasUnterminatedQuote) {
        m_diagnosticEngine.report(DiagnosticID::UnterminatedString)
            .message("Unterminated string literal")
            .span({ m_fileID, invalidLine, invalidColumn })
            .build();
        return addToken(TOK_ERROR);
    }

    return addToken(TOK_STRING);
}

void Lexer::lexSymbol(char32_t cp) {
    std::string symbol = utf8::encodeCodepoint(cp);

    size_t invalidLine = m_startLine;
    size_t invalidColumn = m_startColumn;

    auto symbolIt = g_symbolMap.find(symbol);
    if (symbolIt == g_symbolMap.end()) {
        m_diagnosticEngine.report(DiagnosticID::UnrecognizedSymbol)
            .message("Unrecogized symbol `{}`", symbol)
            .span({ m_fileID, invalidLine, invalidColumn })
            .build();
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
        m_startLine = m_line;
        m_startColumn = m_column;
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
    m_start = 0; m_pos = 0; m_line = 1; m_column = 1; m_startLine = 1; m_startColumn = 1;

    return m_tokens;
}
