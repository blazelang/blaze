#pragma once

#include <string>
#include <unordered_map>

enum TokenKind {
    // Keywords
    TOK_LET,                    // "let"
    TOK_CONST,                  // "const"
    TOK_FUNCTION,               // "function"
    TOK_RETURN,                 // "return"
    TOK_IF,                     // "if"
    TOK_ELSE_IF,                // "else if"
    TOK_ELSE,                   // "else"
    TOK_WHILE,                  // "while"
    TOK_BREAK,                  // "break"
    TOK_CONTINUE,               // "continue"
    TOK_FOR,                    // "for"
    TOK_TRUE,                   // "true"
    TOK_FALSE,                  // "false"
    TOK_ENUM,                   // "enum"
    TOK_NULL,                   // "null"
    TOK_IMPORT,                 // "import"
    TOK_EXPORT,                 // "export"

    // Primitive Types
    TOK_U8,                     // "u8"
    TOK_U16,                    // "u16"
    TOK_U32,                    // "u32"
    TOK_U64,                    // "u64"
    TOK_U128,                   // "u128"
    TOK_I8,                     // "i8"
    TOK_I16,                    // "i16"
    TOK_I32,                    // "i32"
    TOK_I64,                    // "i64"
    TOK_I128,                   // "i128"
    TOK_F16,                    // "f16"
    TOK_F32,                    // "f32"
    TOK_F64,                    // "f64"
    TOK_CHAR,                   // "char"
    TOK_STRING,                 // "string"
    TOK_BOOL,                   // "bool"
    TOK_VOID,                   // "void"

    // Operators
    TOK_ASSIGN,                 // '='  (assignment operator)
    TOK_PLUS,                   // '+'  (addition operator)
    TOK_MINUS,                  // '-'  (subtraction operator)
    TOK_MULTIPLY,               // '*'  (multiplication operator)
    TOK_DIVIDE,                 // '/'  (division operator)
    TOK_MODULO,                 // '%'  (modulus operator)
    TOK_INCREMENT,              // '++' (increment operator)
    TOK_DECREMENT,              // '--' (decrement operator)
    TOK_BITWISE_AND,            // '&'  (bitwise AND)
    TOK_BITWISE_OR,             // '|'  (bitwise OR)
    TOK_BITWISE_XOR,            // '^'  (bitwise XOR)
    TOK_BITWISE_NOT,            // '~'  (bitwise NOT)
    TOK_LEFT_SHIFT,             // '<<' (left shift operator)
    TOK_RIGHT_SHIFT,            // '>>' (right shift operator)
    TOK_EQUAL,                  // '==' (equality comparison)
    TOK_NOT_EQUAL,              // '!=' (inequality comparison)
    TOK_LESS_THAN,              // '<'  (less than comparison)
    TOK_GREATER_THAN,           // '>'  (greater than comparison)
    TOK_LESS_EQUAL,             // '<=' (less than or equal comparison)
    TOK_GREATER_EQUAL,          // '>=' (greater than or equal comparison)
    TOK_LOGICAL_AND,            // '&&' (logical AND)
    TOK_LOGICAL_OR,             // '||' (logical OR)
    TOK_LOGICAL_NOT,            // '!'  (logical NOT)
    TOK_TERNARY_CONDITIONAL,    // '?'  (ternary conditional operator)

    // Compound Assignment
    TOK_PLUS_ASSIGN,            // '+='
    TOK_MINUS_ASSIGN,           // '-='
    TOK_MULTIPLY_ASSIGN,        // '*='
    TOK_DIVIDE_ASSIGN,          // '/='
    TOK_MODULO_ASSIGN,          // '%='
    TOK_AND_ASSIGN,             // '&='
    TOK_OR_ASSIGN,              // '|='
    TOK_XOR_ASSIGN,             // '^='
    TOK_LEFT_SHIFT_ASSIGN,      // '<<='
    TOK_RIGHT_SHIFT_ASSIGN,     // '>>='

    // Access and Scope
    TOK_DOT,                    // '.'  (dot for member access)
    TOK_ARROW,                  // '->' (pointer or lambda member access)

    // Separators and Punctuation
    TOK_COMMA,                  // ','  (comma separator)
    TOK_COLON,                  // ':'  (colon separator)
    TOK_SEMICOLON,              // ';'  (semicolon separator)

    // Parentheses and Brackets
    TOK_LPAREN,                 // "("
    TOK_RPAREN,                 // ")"
    TOK_LBRACE,                 // "{"
    TOK_RBRACE,                 // "}"
    TOK_LBRACKET,               // "["
    TOK_RBRACKET,               // "]"

    // Literals
    TOK_INTEGER_LITERAL,        // e.g., 42, 1000
    TOK_FLOAT_LITERAL,          // e.g., 3.14, 0.1
    TOK_CHAR_LITERAL,           // e.g., 'A', 'B'
    TOK_STRING_LITERAL,         // e.g., "Hello", "World"

    // Identifiers
    TOK_IDENTIFIER,             // An identifier (e.g., variable, function name, etc.)

    // Special
    TOK_DOC_COMMENT_LINE_OUTER,
    TOK_DOC_COMMENT_LINE_INNER,
    TOK_DOC_COMMENT_BLOCK_OUTER,
    TOK_DOC_COMMENT_BLOCK_INNER,
    TOK_ERROR,
    TOK_EOF
};

struct Token {
    TokenKind kind;
    std::pair<size_t, size_t> position;
    std::string lexeme;
};

extern std::unordered_map<std::string, TokenKind> g_keywordMap;

extern std::unordered_map<std::string, TokenKind> g_symbolMap;

std::string TokenKindToString(Token token);
