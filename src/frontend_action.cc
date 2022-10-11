#include "src/frontend_action.h"
#include "src/ast_consumer.h"

std::unique_ptr<clang::ASTConsumer> FrontendAction::CreateASTConsumer(
    clang::CompilerInstance &Compiler, llvm::StringRef InFile) {
  return std::make_unique<ASTConsumer>(&Compiler.getASTContext());
}