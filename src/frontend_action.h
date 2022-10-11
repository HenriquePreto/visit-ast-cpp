#ifndef CC_AST_TOOL_FRONTEND_ACTION_H_
#define CC_AST_TOOL_FRONTEND_ACTION_H_

#include <memory>
#include "clang/AST/ASTConsumer.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"

class FrontendAction : public clang::ASTFrontendAction {
public:
  std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
    clang::CompilerInstance &Compiler, llvm::StringRef InFile) override;
};

#endif // CC_AST_TOOL_FRONTEND_ACTION_H_