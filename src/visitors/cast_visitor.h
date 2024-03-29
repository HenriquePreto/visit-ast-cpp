#ifndef CC_AST_TOOL_CAST_VISITOR_H_
#define CC_AST_TOOL_CAST_VISITOR_H_

#ifndef RAPIDJSON_HAS_STDSTRING
#define RAPIDJSON_HAS_STDSTRING 1
#endif

#include <string>
#include <unordered_map>
#include <unordered_set>

#include "clang/AST/RecursiveASTVisitor.h"
#include "rapidjson/PrettyWriter.h"

class CastVisitor : public clang::RecursiveASTVisitor<CastVisitor> {
public:
  struct FunctionInfo {
    unsigned num_vars_ = 0;
    unsigned begin_line_ = 0;
    unsigned end_line_ = 0;
    std::unordered_multimap<unsigned, clang::CastKind> cast_lines_;
    std::unordered_set<clang::CastKind> cast_kinds_;

    inline unsigned CalculateFunctionSize() const {
      return end_line_ - begin_line_ + 1;
    }
  };

  struct Info {
    std::unordered_map<std::string, FunctionInfo> function_info_;
    
    inline unsigned GetNumCasts(const std::string &function_id) const {
      return function_info_.at(function_id).cast_lines_.size();
    }

    inline unsigned GetNumVars(const std::string &function_id) const {
      return function_info_.at(function_id).num_vars_;
    }

    std::vector<std::pair<const unsigned, clang::CastKind>> GetCastLines(
      const std::string &function_id) const;

    std::vector<clang::CastKind> GetCastKinds(
      const std::string &function_id) const;

    inline unsigned GetFunctionSize(const std::string &function_id) const {
      return function_info_.at(function_id).CalculateFunctionSize();
    }

    inline unsigned GetNumFunctions() const {
      return function_info_.size() - 1; // remove the global context
    }

    void ToJson(
      rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer) const;
  };

  explicit CastVisitor(clang::ASTContext &ctx, Info &info)
    : ctx_(ctx), info_(info),
      current_function_info_(&info.function_info_[""]) {}

  bool VisitFunctionDecl(const clang::FunctionDecl *decl);

  bool VisitImplicitCastExpr(const clang::ImplicitCastExpr *expr);

  bool VisitVarDecl(const clang::VarDecl *decl);

  bool IsValidImplicitCast(const clang::CastKind &cast_kind) const;

  bool IsLocalStmt(const clang::Stmt *stmt) const;

private:
  clang::ASTContext &ctx_;
  Info &info_;
  FunctionInfo *current_function_info_;
};

#endif // CC_AST_TOOL_CAST_VISITOR_H_