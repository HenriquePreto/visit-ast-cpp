#ifndef CC_AST_TOOL_FRONTEND_ACTION_H_
#define CC_AST_TOOL_FRONTEND_ACTION_H_

#include <memory>

#include "src/ast_consumer.h"

#include "clang/AST/ASTConsumer.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Basic/Diagnostic.h"

template <typename T>
class FrontendAction : public clang::ASTFrontendAction {
  using VisitorInfo = typename T::VisitorInfo; 

  public:
    explicit FrontendAction(VisitorInfo& visitor_info)
      : visitor_info_(visitor_info) {}

    std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
      clang::CompilerInstance& instance, llvm::StringRef) override;
    
  private:
    VisitorInfo& visitor_info_;
};

template <typename T>
std::unique_ptr<clang::ASTConsumer> FrontendAction<T>::CreateASTConsumer(
    clang::CompilerInstance& instance, llvm::StringRef) {
  instance.getDiagnostics().setClient(
    new clang::IgnoringDiagConsumer(), /* ShouldOwnClient = */ true);
  return std::make_unique<ASTConsumer<T>>(
    instance.getASTContext(), visitor_info_);
}

#endif // CC_AST_TOOL_FRONTEND_ACTION_H_