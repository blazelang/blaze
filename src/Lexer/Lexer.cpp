#include "Lexer/Lexer.hpp"

#include <string>
#include <format>
#include <stdexcept>

#include "Lexer/Token.hpp"

std::unordered_map<std::string, TokenKind> Lexer::m_keywords = {
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

std::unordered_map<std::string, TokenKind> Lexer::m_symbols = {
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

bool Lexer::match(char32_t c) {
    if (isEnd() || c != peek()) {
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

void Lexer::addToken(TokenKind kind) {
    size_t column = m_column - (m_pos - m_start);
    m_tokens.push_back({
        .kind = kind,
        .position = std::pair(m_line, column),
        .lexeme = std::string(m_source.substr(m_start, m_pos - m_start))
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

bool Lexer::isDecimalDigit(char32_t codepoint) const {
    return (codepoint >= U'0' && codepoint <= U'9');
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

void Lexer::lexSymbol(char32_t codepoint) {
    // Start with the first character
    std::string symbol = codepointToString(codepoint);

    // Check if the symbol exist
    auto symbolIt = m_symbols.find(symbol);
    if (symbolIt == m_symbols.end()) {
        reportError(std::format("Unrecognized symbol: {}", codepointToString(codepoint)));
        return addToken(TOK_ERROR);
    }

    // Try to extend the symbol as long as it's exist
    while (!isEnd()) {
        // Check if the extended symbol exist
        auto extendedSymbolIt = m_symbols.find(symbol + codepointToString(peek()));
        if (extendedSymbolIt != m_symbols.end()) {
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
