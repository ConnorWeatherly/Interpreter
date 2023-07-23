#ifndef __SIMPLIFY_AST_H__
#define __SIMPLIFY_AST_H__

#include "build_ast.h"

// Check the sign of integers
#define DIFF_SIGN(n1, n2) (((int64_t)n1^(int64_t)n2)>>63)

// Check the value of double
#define INF_MASK(n)   ((*(uint64_t*)&n)&0x7fffffffffffffff)
#define IS_DBL_INF(n) (INF_MASK(n) == 0x7ff0000000000000)

void simplify_ast(AST ast, ErrorReport* report);

#endif
