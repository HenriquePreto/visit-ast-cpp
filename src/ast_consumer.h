#ifndef CC_AST_TOOL_AST_CONSUMER_H_
#define CC_AST_TOOL_AST_CONSUMER_H_

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/Basic/SourceManager.h"

template <typename T>
class ASTConsumer : public clang::ASTConsumer {
public:
  explicit ASTConsumer(clang::ASTContext &ctx, 
                       typename T::VisitorInfo &visitor_info)
    : visitor_(ctx, visitor_info) {}

  void HandleTranslationUnit(clang::ASTContext &ctx) override;

  bool IsFromSystemHeader(clang::SourceManager &src_mgr, 
                          const clang::Decl *decl) const;

private:
  T visitor_;
};

template <typename T>
void ASTConsumer<T>::HandleTranslationUnit(clang::ASTContext &ctx) {
  auto *decl = ctx.getTranslationUnitDecl();
  auto &src_mgr = ctx.getSourceManager();

  auto begin = decl->decls_begin();
  auto end = decl->decls_end();
  auto it = std::find_if(begin, end, [this, &src_mgr] (auto *child_decl) {
    return !IsFromSystemHeader(src_mgr, child_decl);
  });

  for (auto continue_traversal = true; it != end && continue_traversal; ++it) {
    if (!IsFromSystemHeader(src_mgr, *it)) {
      continue_traversal = visitor_.TraverseDecl(*it);
    }
  }
}

template <typename T>
bool ASTConsumer<T>::IsFromSystemHeader(
  clang::SourceManager &src_mgr, const clang::Decl *decl) const {
  return src_mgr.isInSystemHeader(decl->getLocation()) ||
         src_mgr.isInSystemMacro(decl->getLocation());
}

#endif // CC_AST_TOOL_AST_CONSUMER_H_