#include <optional>
#include <iostream>

#include "Lexer/Lexer.hpp"
#include "Lexer/Token.hpp"
#include "SourceManager/SourceManager.hpp"
#include "Diagnostics/DiagnosticEngine.hpp"

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: compiler <file>\n";
        return EXIT_FAILURE;
    }

    SourceManager sourceManager;
    std::optional<ISourceManager::FileID> sourceFileID = sourceManager.loadFile(argv[1]);
    if (!sourceFileID.has_value()) {
        std::cerr << "Failed to load file: " << argv[1] << '\n';
        return EXIT_FAILURE;
    }

    // Diagnostic engine tracks all warnings/errors across all compiler phases
    DiagnosticEngine diagnosticEngine(sourceManager);

    // Phase 1: Lexical Analysis (Tokenization)
    Lexer lexer(sourceFileID.value(), sourceManager, diagnosticEngine);
    std::vector<Token>& tokens = lexer.tokenize();

    // Print all tokens generated from lexer
    // for (const auto& token : tokens) {
    //     std::cout << std::format("Token: {}", TokenKindToString(token)) << '\n';
    // }

    diagnosticEngine.printDiagnostics();

    // skip codegen if any errors occurred
    if (diagnosticEngine.hasErrors()) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
