#include "Lexer/Lexer.hpp"

#include <string>
#include <format>
#include <stdexcept>
#include <string_view>

#include "Lexer/Token.hpp"

std::unordered_map<std::string, TokenKind> Lexer::s_keywords = {
    {"let", TOK_LET},
    {"const", TOK_CONST},
    {"fn", TOK_FUNCTION},
    {"return", TOK_RETURN},
    {"if", TOK_IF},
    {"else if", TOK_ELSE_IF},
    {"else", TOK_ELSE},
    {"while", TOK_WHILE},
    {"break", TOK_BREAK},
    {"continue", TOK_CONTINUE},
    {"for", TOK_FOR},
    {"true", TOK_TRUE},
    {"false", TOK_FALSE},
    {"enum", TOK_ENUM},
    {"null", TOK_NULL},
    {"import", TOK_IMPORT},
    {"export", TOK_EXPORT},
    {"u8", TOK_U8},
    {"u16", TOK_U16},
    {"u32", TOK_U32},
    {"u64", TOK_U64},
    {"u128", TOK_U128},
    {"i8", TOK_I8},
    {"i16", TOK_I16},
    {"i32", TOK_I32},
    {"i64", TOK_I64},
    {"i128", TOK_I128},
    {"f16", TOK_F16},
    {"f32", TOK_F32},
    {"f64", TOK_F64},
    {"char", TOK_CHAR},
    {"string", TOK_STRING},
    {"bool", TOK_BOOL},
    {"void", TOK_VOID},
};

std::unordered_map<std::string, TokenKind> Lexer::s_symbols = {
    {",", TOK_COMMA},
    {":", TOK_COLON},
    {";", TOK_SEMICOLON},
    {".", TOK_DOT},
    {"~", TOK_BITWISE_NOT},
    {"(", TOK_LPAREN},
    {")", TOK_RPAREN},
    {"{", TOK_LBRACE},
    {"}", TOK_RBRACE},
    {"[", TOK_LBRACKET},
    {"]", TOK_RBRACKET},
    {"?", TOK_TERNARY_CONDITIONAL},
    {"=", TOK_ASSIGN},
    {"+", TOK_PLUS},
    {"-", TOK_MINUS},
    {"*", TOK_MULTIPLY},
    {"/", TOK_DIVIDE},
    {"%", TOK_MODULO},
    {"&", TOK_BITWISE_AND},
    {"|", TOK_BITWISE_OR},
    {"^", TOK_XOR_ASSIGN},
    {"<", TOK_LESS_THAN},
    {">", TOK_GREATER_THAN},
    {"!", TOK_LOGICAL_NOT},
    {"!=", TOK_NOT_EQUAL},
    {"==", TOK_EQUAL},
    {"->", TOK_ARROW},
    {"+=", TOK_PLUS_ASSIGN},
    {"++", TOK_INCREMENT},
    {"-=", TOK_MINUS_ASSIGN},
    {"--", TOK_DECREMENT},
    {"*=", TOK_MULTIPLY_ASSIGN},
    {"/=", TOK_DIVIDE_ASSIGN},
    {"%=", TOK_MODULO_ASSIGN},
    {"&=", TOK_AND_ASSIGN},
    {"&&", TOK_LOGICAL_AND},
    {"|=", TOK_OR_ASSIGN},
    {"||", TOK_LOGICAL_OR},
    {"^=", TOK_XOR_ASSIGN},
    {"<=", TOK_LESS_EQUAL},
    {"<<=", TOK_LEFT_SHIFT_ASSIGN},
    {"<<", TOK_LEFT_SHIFT},
    {">=", TOK_GREATER_EQUAL},
    {">>=", TOK_RIGHT_SHIFT_ASSIGN},
    {">>", TOK_RIGHT_SHIFT}
};

Lexer::Lexer(
    ISourceManager::FileID fileID,
    ISourceManager& sourceManager,
    IDiagnosticEngine& diagnosticEngine
) :
    m_fileID(fileID),
    m_sourceManager(sourceManager),
    m_diagnosticEngine(diagnosticEngine),
    m_source(sourceManager.getBuffer(fileID)),
    m_start(0),
    m_pos(0),
    m_line(1),
    m_column(1) {}

Lexer::CodepointInfo Lexer::decodeUTF8(int pos) const {
    char32_t codepoint = 0;
    uint8_t byteLength = 0;

    if (isEnd()) {
        return { .codepoint = codepoint, .byteLength = byteLength };
    }

    if ((m_source.at(pos) & 0x80) == 0x0) {
        codepoint = m_source.at(pos);
        byteLength = 1;
    }

    else if ((m_source.at(pos) & 0xE0) == 0xC0) {
        codepoint = ((m_source.at(pos) & 0X1F) << 6) | (m_source.at(pos + 1) & 0x3F);
        byteLength = 2;
    }

    else if ((m_source.at(pos) & 0xF0) == 0xE0) {
        codepoint = ((m_source.at(pos) & 0XF) << 12) | ((m_source.at(pos + 1) & 0x3F) << 6) | (m_source.at(pos + 2) & 0x3F);
        byteLength = 3;
    }

    else if ((m_source.at(pos) & 0xF8) == 0xF0) {
        codepoint = ((m_source.at(pos) & 0X7) << 18) | ((m_source.at(pos + 1) & 0x3F) << 12) | ((m_source.at(pos + 2) & 0x3F) << 6) | (m_source.at(pos + 3) & 0x3F);
        byteLength = 4;
    }

    else {
        throw std::invalid_argument("Invalid UTF-8 byte sequence");
    }

    return { .codepoint = codepoint, .byteLength = byteLength };
}

std::string Lexer::codepointToString(char32_t codepoint) const {
    std::string result;

    // Handle 1-byte UTF-8 (ASCII range: U+0000 to U+007F)
    if (codepoint <= 0x7F) {
        result.push_back(static_cast<char>(codepoint));
    }

    // Handle 2-byte UTF-8 (U+0080 to U+07FF)
    else if (codepoint <= 0x7FF) {
        result.push_back(static_cast<char>(0xC0 | ((codepoint >> 6) & 0x1F)));
        result.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
    }

    // Handle 3-byte UTF-8 (U+0800 to U+FFFF)
    else if (codepoint <= 0xFFFF) {
        result.push_back(static_cast<char>(0xE0 | ((codepoint >> 12) & 0x0F)));
        result.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
        result.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
    }

    // Handle 4-byte UTF-8 (U+10000 to U+10FFFF)
    else if (codepoint <= 0x10FFFF) {
        result.push_back(static_cast<char>(0xF0 | ((codepoint >> 18) & 0x07)));
        result.push_back(static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F)));
        result.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
        result.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
    }

    return result;
}

char32_t Lexer::advance() {
    CodepointInfo codepointInfo = decodeUTF8(m_pos);
    m_pos += codepointInfo.byteLength;
    m_column += 1;
    return codepointInfo.codepoint;
};

char32_t Lexer::peek() const {
    return decodeUTF8(m_pos).codepoint;
};

char32_t Lexer::lookahead(int i) const {
    if (m_pos + i >= m_source.length()) {
        return '\0';
    }
    return decodeUTF8(m_pos + i).codepoint;
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

void Lexer::addToken(TokenKind kind) {
    size_t column = m_column - (m_pos - m_start);
    m_tokens.push_back({
        .kind = kind,
        .position = std::pair(m_line, column),
        .lexeme = std::string(getLexeme())
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

bool Lexer::isNum(char32_t codepoint) const {
    return isDecimalDigit(codepoint) || (codepoint == U'.' && isDecimalDigit(peek())) || (codepoint == U'_' && isDecimalDigit(peek()));
}

bool Lexer::isAlpha(char32_t codepoint) const {
    return (codepoint >= 'a' && codepoint <= 'z') || (codepoint >= 'A' && codepoint <= 'Z') || codepoint == '_';
}

bool Lexer::isAlphaNum(char32_t codepoint) const {
    return isAlpha(codepoint) || isDecimalDigit(codepoint);
}

void Lexer::skipWhitespace(char32_t codepoint) {
    if (codepoint == U'\n') {
        m_line += 1;
        m_column = 1;
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
            advance();
            advance();
        } else if (lookahead() == U'1') {
            if (lookahead(2) == U'6') {
                advance();
                advance();
                advance();
            } else if (lookahead(2) == U'2' && lookahead(3) == U'8') {
                advance();
                advance();
                advance();
                advance();
            }
        } else if (lookahead() == U'3' && lookahead(2) == U'2') {
            advance();
            advance();
            advance();
        } else if (lookahead() == U'6' && lookahead(2) == U'4') {
            advance();
            advance();
            advance();
        }
    }

    // Handle float suffixes like f32, f64
    if (peek() == U'f' || peek() == U'F') {
        isFloat = true;
        if (lookahead() == U'3' && lookahead(2) == U'2') {
            advance();
            advance();
            advance();
        } else if (lookahead() == U'6' && lookahead(2) == U'4') {
            advance();
            advance();
            advance();
        }
    }

    // Catch any remaining invalid suffix
    while (isAlphaNum(peek())) {
        invalidSuffix += advance();
    }

    // === Final validations and error reports ===

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

void Lexer::lexSymbol(char32_t codepoint) {
    // Start with the first character
    std::string symbol = codepointToString(codepoint);

    // Check if the symbol exist
    auto symbolIt = s_symbols.find(symbol);
    if (symbolIt == s_symbols.end()) {
        reportError(std::format("Unrecognized symbol: {}", codepointToString(codepoint)));
        return addToken(TOK_ERROR);
    }

    // Try to extend the symbol as long as it's exist
    while (!isEnd()) {
        // Check if the extended symbol exist
        auto extendedSymbolIt = s_symbols.find(symbol + codepointToString(peek()));
        if (extendedSymbolIt != s_symbols.end()) {
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
        const char32_t codepoint = advance();
        if (isWhitespace(codepoint)) {
            skipWhitespace(codepoint);
        } else if (isNum(codepoint)) {
            lexNumberLiteral(codepoint);
        } else {
            lexSymbol(codepoint);
        }
    }

    m_start = m_pos;
    addToken(TOK_EOF);

    // Reset values
    m_start = 0; m_pos = 0; m_line = 1; m_column = 1;

    return m_tokens;
}
