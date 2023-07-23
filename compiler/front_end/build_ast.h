#ifndef __BUILD_AST_H__
#define __BUILD_AST_H__

#include "ast_structs.h"
#include "error_report.h"

#define EMPTY_AST_NODE (ASTNode) {  \
  .oprtr    = NOP,                  \
  .oprtrPos = NULL,                 \
  .prev     = NULL,                 \
                                    \
  .left = (ASTBranch) {             \
    .dataType = NONE,               \
    .mutation = NRMAL,              \
    .branch   = NULL,               \
  },                                \
  .right = (ASTBranch) {            \
    .dataType = NONE,               \
    .mutation = NRMAL,              \
    .branch   = NULL,               \
  },                                \
}

// Build an absract synat tree for a code line
AST
build_line_ast(const char* str, ErrorReport* report);

// Free data saved in abstract syntax tree
void
free_ast(AST* node);

#endif
