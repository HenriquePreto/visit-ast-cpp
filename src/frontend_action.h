#ifndef CC_AST_TOOL_FRONTEND_ACTION_H_
#define CC_AST_TOOL_FRONTEND_ACTION_H_

#include <memory>

#include "ast_consumer.h"

#include "clang/AST/ASTConsumer.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "rapidjson/PrettyWriter.h"

template <typename T>
class FrontendAction : public clang::ASTFrontendAction {
  using Info = typename T::Info;
public:
  explicit FrontendAction(Info *info) : owner_(false), info_(info) {}

  explicit FrontendAction() : owner_(true), info_(new Info()) {}

  ~FrontendAction() {
    if (owner_) 
      delete info_;
  }

  std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
    clang::CompilerInstance &instance, llvm::StringRef) override;

  void EndSourceFile() override;

  inline Info *GetInfo() {
    return info_;
  }
  
private:
  bool owner_;
  Info* info_;
};

template <typename T>
std::unique_ptr<clang::ASTConsumer> FrontendAction<T>::CreateASTConsumer(
    clang::CompilerInstance &instance, llvm::StringRef) {
  return std::make_unique<ASTConsumer<T>>(instance.getASTContext(), *info_);
}

template <typename T>
void FrontendAction<T>::EndSourceFile() {
  if (owner_) {
    rapidjson::StringBuffer string_buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(string_buffer);
    info_->ToJson(writer);
    llvm::outs() << string_buffer.GetString() << '\n';
  }
}

#endif // CC_AST_TOOL_FRONTEND_ACTION_H_