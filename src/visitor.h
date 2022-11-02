#ifndef CC_AST_TOOL_VISITOR_H_
#define CC_AST_TOOL_VISITOR_H_

#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/ASTContext.h"
#include "clang/Basic/SourceManager.h"

template <typename T>
class Visitor : public clang::RecursiveASTVisitor<T> {
public:
  explicit Visitor(clang::ASTContext &ctx)
    : ctx_(ctx) {}

  bool TraverseDecl(clang::Decl *decl);
  
  bool TraverseStmt(clang::Stmt *stmt);

  bool IsTraversable(const clang::SourceLocation &begin_loc) const;

protected:
  clang::ASTContext &ctx_;
};

template <typename T>
bool Visitor<T>::TraverseDecl(clang::Decl *decl) {
  auto begin_loc = decl->getBeginLoc();
  auto kind = decl->getKind();
  if (IsTraversable(begin_loc) ||
      kind == clang::Decl::Kind::TranslationUnit) {
    return clang::RecursiveASTVisitor<T>::TraverseDecl(decl);
  }
  return true;
}

template <typename T>
bool Visitor<T>::TraverseStmt(clang::Stmt *stmt) {
  auto begin_loc = stmt->getBeginLoc();
  if (IsTraversable(begin_loc)) {
    return clang::RecursiveASTVisitor<T>::TraverseStmt(stmt);
  }
  return true;
}

template <typename T>
bool Visitor<T>::IsTraversable(const clang::SourceLocation &begin_loc) const {
  auto &src_mgr = ctx_.getSourceManager();
  return src_mgr.isInMainFile(begin_loc);
}


#endif // CC_AST_TOOL_VISITOR_H_