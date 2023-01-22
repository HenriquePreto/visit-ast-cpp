#ifndef CC_AST_TOOL_FRONTEND_ACTION_H_
#define CC_AST_TOOL_FRONTEND_ACTION_H_

#include <memory>

#include "ast_consumer.h"

#include "clang/AST/ASTConsumer.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Basic/Diagnostic.h"

template <typename T>
class FrontendAction : public clang::ASTFrontendAction {
  using Info = typename T::Info;

public:
  explicit FrontendAction(Info &info, bool ignore_errors)
    : info_(info), ignore_errors_(ignore_errors) {}

  std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
    clang::CompilerInstance &instance, llvm::StringRef) override;
  
private:
  Info &info_;
  bool ignore_errors_;
};

template <typename T>
std::unique_ptr<clang::ASTConsumer> FrontendAction<T>::CreateASTConsumer(
    clang::CompilerInstance &instance, llvm::StringRef) {
  if (ignore_errors_)
    instance.getDiagnostics().setClient(
      new clang::IgnoringDiagConsumer(), /* ShouldOwnClient = */ true);
  return std::make_unique<ASTConsumer<T>>(
    instance.getASTContext(), info_);
}

#endif // CC_AST_TOOL_FRONTEND_ACTION_H_