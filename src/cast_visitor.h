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
    class CastInfo {
      public:
        int num_casts_ = 0;
        int num_vars_ = 0;
        std::unordered_set<clang::CastKind> cast_kinds_;
    };

    class VisitorInfo {
      public:
        std::unordered_map<std::string, CastInfo> function_info_;
        
        inline int GetNumCasts(const std::string& function_name) const {
          return function_info_.at(function_name).num_casts_;
        }

        inline int GetNumVars(const std::string& function_name) const {
          return function_info_.at(function_name).num_vars_;
        }

        inline std::vector<clang::CastKind> GetCastKinds(
          const std::string& function_name) const {
            auto& ck_set = function_info_.at(function_name).cast_kinds_;
            return std::vector(ck_set.cbegin(), ck_set.cend());
        }

        inline int GetNumFunctions() const {
          return function_info_.size();
        }

        void ToJson(
          rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) const;
    };

    explicit CastVisitor(clang::ASTContext& ctx, VisitorInfo& visitor_info)
      : ctx_(ctx), visitor_info_(visitor_info) {}

    bool VisitFunctionDecl(const clang::FunctionDecl* decl);

    bool VisitImplicitCastExpr(const clang::ImplicitCastExpr* expr);

    bool VisitVarDecl(const clang::VarDecl* decl);

    bool IsValidImplicitCast(const clang::CastKind& cast_kind) const;

  private:
    clang::ASTContext& ctx_;
    std::string current_function_;
    VisitorInfo& visitor_info_;
};

#endif // CC_AST_TOOL_CAST_VISITOR_H_