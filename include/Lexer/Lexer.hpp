#pragma once

#include <optional>
#include <vector>
#include <string_view>

#include "Lexer/Token.hpp"
#include "SourceManager/ISourceManager.hpp"
#include "Diagnostics/IDiagnosticEngine.hpp"

class Lexer {
public:
    Lexer(ISourceManager::FileID fileID, ISourceManager& sourceManager, IDiagnosticEngine& diagnosticEngine);

    std::vector<Token>& tokenize();

private:
    enum NumericBase {
        Binary = 2,
        Octal = 8,
        Decimal = 10,
        HexaDecimal = 16
    };

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

    ISourceManager::FileID m_fileID;
    ISourceManager& m_sourceManager;
    IDiagnosticEngine& m_diagnosticEngine;
    std::string_view m_source;
    size_t m_start;
    size_t m_pos;
    size_t m_line;
    size_t m_column;
    std::vector<Token> m_tokens;

    char32_t advance();
    char32_t peek() const;
    char32_t lookahead(int i = 1) const;
    bool match(const char32_t cp);

    SourcePosition currentPosition() const;
    void reportError(const std::string& message);
    void reportWarning(const std::string& message);

    std::string_view getLexeme() const;
    void addToken(TokenKind kind, std::optional<std::string> lexeme = std::nullopt);

    std::string normalizeToNFKC(const std::string_view lexeme);

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

    /// Checks for a valid escape sequence at the current position.
    /// Returns the number of characters to consume. Sets error flags if invalid.
    bool matchEscapeSequence(EscapeSequenceErrorKind& errorKind);

    void skipWhitespace(char32_t cp);
    void lexLineComment();
    void lexBlockComment();
    void lexKeywordOrIdentifier();
    void lexNumberLiteral(char32_t cp);
    void lexCharLiteral();
    void lexStringLiteral();
    void lexSymbol(char32_t cp);
};
