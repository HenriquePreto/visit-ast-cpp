#ifndef CC_AST_TOOL_CASTS_VISITOR_H_
#define CC_AST_TOOL_CASTS_VISITOR_H_

#include "clang/AST/RecursiveASTVisitor.h"
#include <iostream>
#include <string>
#include <map>

class CastsVisitor : public clang::RecursiveASTVisitor<CastsVisitor> {
  public:
    class CastsInfo {
      public:
        int num_casts = 0;
        int num_vars = 0;

        friend std::ostream& operator<<(std::ostream& os, const CastsInfo& casts_info) {
          os << "(num_casts=" 
             << casts_info.num_casts 
             << ", num_vars=" 
             << casts_info.num_vars 
             << ", freq="
             << static_cast<double>(casts_info.num_casts) / casts_info.num_vars
             << ")";
          return os;
        }
    };

    class Collector {
      public:
        std::map<std::string, CastsInfo> function_info_;
        
        friend std::ostream& operator<<(std::ostream& os, const Collector& collector) {
          os << "CastVisitorInfo {" << std::endl;
          for (auto const& [key, val] : collector.function_info_) {
            os << "\t"
               << key
               << " -> "
               << val
               << ", "
               << std::endl;
          }
          os << "}";  
          return os;
        }
    };

    explicit CastsVisitor(clang::ASTContext& ctx, Collector& collector)
      : ctx_(ctx), collector_(collector) {}

    bool VisitFunctionDecl(const clang::FunctionDecl* decl);

    bool VisitCastExpr(const clang::CastExpr* expr);

    bool VisitVarDecl(const clang::VarDecl* decl);

  private:
    clang::ASTContext& ctx_;
    std::string current_function_;
    Collector& collector_;
};

#endif // CC_AST_TOOL_CASTS_VISITOR_H_