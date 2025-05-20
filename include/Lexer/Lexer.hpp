#pragma once

#include <vector>
#include <optional>
#include <string_view>

#include "Lexer/Token.hpp"
#include "SourceManager/SourceManager.hpp"
#include "Diagnostics/DiagnosticEngine.hpp"

class Lexer {
public:
    enum class EscapeSequenceErrorKind {
        UnknownEscape,
        NumericEscapeTooShort,
        InvalidNumericEscape,
        HexOutOfRange,
        MalformedUnicodeSequence,
        EmptyUnicodeEscape,
        InvalidUnicodeChar,
        UnterminatedUnicode,
        OverlongUnicodeChar,
        UnicodeOutOfRange,
    };

    Lexer(ISourceManager::FileID fileID, ISourceManager& sourceManager, IDiagnosticEngine& diagnosticEngine);

    std::vector<Token>& tokenize();

private:
    enum NumericBase {
        Binary = 2,
        Octal = 8,
        Decimal = 10,
        HexaDecimal = 16
    };

    // External dependencies
    ISourceManager::FileID m_fileID;
    ISourceManager& m_sourceManager;
    IDiagnosticEngine& m_diagnosticEngine;

    // source code data
    std::string_view m_source;

    // current position state
    size_t m_start = 0;     // start index of current token
    size_t m_pos = 0;       // current index in source
    size_t m_line = 1;      // current line number
    size_t m_column = 1;    // current column number

    // start location of current token
    size_t m_startLine = 1;
    size_t m_startColumn = 1;

    std::vector<Token> m_tokens;

    // Core lexing methods
    char32_t advance();
    char32_t peek() const;
    char32_t lookahead(int i = 1) const;
    bool match(const char32_t cp);

    std::string_view getLexeme() const;

    void addToken(TokenKind kind, std::optional<std::string> lexeme = std::nullopt);

    // Character classification
    bool isEnd() const;
    bool isWhitespace(char32_t cp) const;
    bool isHexDigit(char32_t cp) const;
    bool isOctalDigit(char32_t cp) const;
    bool isBinaryDigit(char32_t cp) const;
    bool isDecimalDigit(char32_t cp) const;
    bool isNumberStart(char32_t cp) const;
    bool isDigit(Lexer::NumericBase base, char32_t cp) const;
    bool isAlpha(char32_t cp) const;
    bool isAlphaNum(char32_t cp) const;
    bool isIdentifierStart(char32_t cp);
    bool isIdentifierContinue(char32_t cp);

    // Escape sequence handling
    bool matchEscapeSequence(EscapeSequenceErrorKind& errorKind);

    // Lexing functions
    void skipWhitespace(char32_t cp);
    void lexLineComment();
    void lexBlockComment();
    void lexKeywordOrIdentifier();
    void lexNumberLiteral(char32_t cp);
    void lexCharLiteral();
    void lexStringLiteral();
    void lexSymbol(char32_t cp);
};
