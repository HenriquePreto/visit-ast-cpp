#ifndef CC_AST_TOOL_FRONTEND_ACTION_H_
#define CC_AST_TOOL_FRONTEND_ACTION_H_

#include <memory>
#include "clang/AST/ASTConsumer.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Basic/Diagnostic.h"
#include "src/ast_consumer.h"

template <typename T>
class FrontendAction : public clang::ASTFrontendAction {
public:
  std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
    clang::CompilerInstance &Compiler, llvm::StringRef InFile) override;
};

template <typename T>
std::unique_ptr<clang::ASTConsumer> FrontendAction<T>::CreateASTConsumer(
    clang::CompilerInstance &Compiler, llvm::StringRef InFile) {
  Compiler.getDiagnostics().setClient(new clang::IgnoringDiagConsumer(), /* ShouldOwnClient = */ true);
  return std::make_unique<ASTConsumer<T>>(&Compiler.getASTContext());
}

#endif // CC_AST_TOOL_FRONTEND_ACTION_H_