#include "errors.h"
#include "parser.h"
#include "simplify_ast.h"

#include <float.h>
#include <stdlib.h>

#include <stdio.h>

/* Functions for error checking during simplification */
static double
_abs_dbl(double dbl) {
  return (dbl > 0) ? dbl : -dbl;
}

static uint64_t
_abs_int64(int64_t int64) {
  return (int64 > 0) ? int64 : -int64;
}

static bool
_abs_dbl_not_decimal(double dbl) {
  return dbl >= 1;
}

static DataType
_find_int_type(ASTNode* node, int64_t num) {
  if (num == (int32_t)num) {
    node->left.value.asInt32 = num;
    return INTGR32;
  }

  node->left.value.asInt64 = num;
  return INTGR64;
}

static DataType
_find_flt_type(ASTNode* node, double num) {
  if (num == (float)num) {
    node->left.value.asFloat = num;
    return FLT;
  }

  node->left.value.asDouble = num;
  return DBL;
}

/* Functions for simplifying abstract syntax tree */
static bool
_mlt(ASTNode* node, ErrorReport* report) {
  switch (node->left.dataType) {
    case INTGR32:
    case INTGR64: {
      switch (node->right.dataType) {
        case INTGR32:
        case INTGR64: {
          // Check for overflow past 64 bits
          uint64_t absLeft  = _abs_int64(node->left.value.asInt64);
          uint64_t absRight = _abs_int64(node->right.value.asInt64);
          if ((absLeft != 0) && (INT64_MAX/absLeft < absRight)) {
            *report = ERR_OPP_INT_OVERFLOW(node->oprtrPos, node->oprtrPos+1);
            return false;
          }

          // Set data type to smallest possible size
          node->left.dataType = _find_int_type(node,
            node->left.value.asInt64*node->right.value.asInt64);

          return true;
        }
        case FLT:
        case DBL: {
          // Check for overflow past double
          uint64_t  absLeft  = _abs_int64(node->left.value.asInt64);
          double    absRight = _abs_dbl(node->right.value.asDouble);
          if ((absLeft != 0) && (DBL_MAX/absLeft < absRight)) {
            *report = ERR_OPP_DBL_OVERFLOW(node->oprtrPos, node->oprtrPos+1);
            return false;
          }

          // Set data type to smallest possible size
          node->left.dataType = _find_flt_type(node,
            node->left.value.asInt64*node->right.value.asDouble);

          return true;
        }
        default: {
          break;
        }
      }
      break;
    }
    case FLT:
    case DBL: {
      switch (node->right.dataType) {
        case INTGR32:
        case INTGR64: {
          // Check for overflow past double
          double    absLeft  = _abs_dbl(node->left.value.asDouble);
          uint64_t  absRight = _abs_int64(node->right.value.asInt64);
          if ((absRight != 0) && (DBL_MAX/absRight < absLeft)) {
            *report = ERR_OPP_DBL_OVERFLOW(node->oprtrPos, node->oprtrPos+1);
            return false;
          }

          // Set data type to smallest possible size
          node->left.dataType = _find_flt_type(node,
            node->left.value.asDouble*node->right.value.asInt64);

          return true;
        }
        case FLT:
        case DBL: {
          // Check for overflow past double
          double absLeft  = _abs_dbl(node->left.value.asDouble);
          double absRight = _abs_dbl(node->right.value.asDouble);
          if (_abs_dbl_not_decimal(absLeft) &&
          (absLeft != 0) && (DBL_MAX/absLeft < absRight)) {
            *report = ERR_OPP_DBL_OVERFLOW(node->oprtrPos, node->oprtrPos+1);
            return false;
          }

          // Set data type to smallest possible size
          node->left.dataType = _find_flt_type(node,
            node->left.value.asDouble*node->right.value.asDouble);

          return true;
        }
        default: {
          break;
        }
      }
      break;
    }
    default: {
      break;
    }
  }
  return false;
}

static bool
_div(ASTNode* node, ErrorReport* report) {
  switch (node->left.dataType) {
    case INTGR32:
    case INTGR64: {
      switch (node->right.dataType) {
        case INTGR32:
        case INTGR64: {
          // Check for divide by zero error
          if (node->right.value.asInt64 == 0) {
            *report = ERR_DIV_BY_ZERO(node->oprtrPos, node->oprtrPos+1);
            return false;
          }

          // Set data type to smallest possible size
          node->left.dataType = _find_int_type(node,
            node->left.value.asInt64/node->right.value.asInt64);

          return true;
        }
        case FLT:
        case DBL: {
          // Check for overflow past double
          uint64_t  absLeft  = _abs_int64(node->left.value.asInt64);
          double    absRight = _abs_dbl(node->right.value.asDouble);
          if ((absRight == 0) ||
          (_abs_dbl_not_decimal(absRight) && (DBL_MAX*absRight < absLeft))) {
            *report = (
              (absRight == 0) ?
              ERR_DIV_BY_ZERO(node->oprtrPos, node->oprtrPos+1) :
              ERR_OPP_DBL_OVERFLOW(node->oprtrPos, node->oprtrPos+1)
            );
            return false;
          }

          // Set data type to smallest possible size
          node->left.dataType = _find_flt_type(node,
            node->left.value.asInt64/node->right.value.asDouble);

          return true;
        }
        default: {
          break;
        }
      }
      break;
    }
    case FLT:
    case DBL: {
      switch (node->right.dataType) {
        case INTGR32:
        case INTGR64: {
          // Check for divide by zero error
          if (node->right.value.asInt64 == 0) {
            *report = ERR_DIV_BY_ZERO(node->oprtrPos, node->oprtrPos+1);
            return false;
          }

          // Set data type to smallest possible size
          node->left.dataType = _find_flt_type(node,
            node->left.value.asDouble/node->right.value.asInt64);

          return true;
        }
        case FLT:
        case DBL: {
          // Check for overflow past double
          double absLeft  = _abs_dbl(node->left.value.asDouble);
          double absRight = _abs_dbl(node->right.value.asDouble);
          if ((absRight == 0) ||
          (_abs_dbl_not_decimal(absRight) && (DBL_MAX*absRight < absLeft))) {
            *report = (
              (absRight == 0) ?
              ERR_DIV_BY_ZERO(node->oprtrPos, node->oprtrPos+1) :
              ERR_OPP_DBL_OVERFLOW(node->oprtrPos, node->oprtrPos+1)
            );
            return false;
          }

          // Set data type to smallest possible size
          node->left.dataType = _find_flt_type(node,
            node->left.value.asDouble/node->right.value.asDouble);

          return true;
        }
        default: {
          break;
        }
      }
      break;
    }
    default: {
      break;
    }
  }
  return false;
}

static bool
_add(ASTNode* node, ErrorReport* report) {
  switch (node->left.dataType) {
    case INTGR32:
    case INTGR64: {
      switch (node->right.dataType) {
        case INTGR32:
        case INTGR64: {
          // Check for overflow past 64 bits
          int64_t left  = node->left.value.asInt64;
          int64_t right = node->right.value.asInt64;

          int64_t sum   = left+right;
          if (!DIFF_SIGN(left, right) && DIFF_SIGN(left, sum)) {
            *report = ERR_OPP_INT_OVERFLOW(node->oprtrPos, node->oprtrPos+1);
            return false;
          }

          // Set data type to smallest possible size
          node->left.dataType = _find_int_type(node, sum);

          return true;
        }
        case FLT:
        case DBL: {
          // Check for overflow past double
          double sum = node->left.value.asInt64+node->right.value.asDouble;
          if (IS_DBL_INF(sum)) {
            *report = ERR_OPP_DBL_OVERFLOW(node->oprtrPos, node->oprtrPos+1);
            return false;
          }

          // Set data type to smallest possible size
          node->left.dataType = _find_flt_type(node, sum);

          return true;
        }
        default: {
          break;
        }
      }
      break;
    }
    case FLT:
    case DBL: {
      switch (node->right.dataType) {
        case INTGR32:
        case INTGR64: {
          // Check for overflow past double
          double sum = node->left.value.asDouble+node->right.value.asInt64;
          if (IS_DBL_INF(sum)) {
            *report = ERR_OPP_DBL_OVERFLOW(node->oprtrPos, node->oprtrPos+1);
            return false;
          }

          // Set data type to smallest possible size
          node->left.dataType = _find_flt_type(node, sum);

          return true;
        }
        case FLT:
        case DBL: {
          // Check for overflow past double
          double sum = node->left.value.asDouble+node->right.value.asDouble;
          if (IS_DBL_INF(sum)) {
            *report = ERR_OPP_DBL_OVERFLOW(node->oprtrPos, node->oprtrPos+1);
            return false;
          }

          // Set data type to smallest possible size
          node->left.dataType = _find_flt_type(node, sum);

          return true;
        }
        default: {
          break;
        }
      }
      break;
    }
    default: {
      break;
    }
  }
  return false;
}

static bool
_sub(ASTNode* node, ErrorReport* report) {
  switch (node->left.dataType) {
    case INTGR32:
    case INTGR64: {
      switch (node->right.dataType) {
        case INTGR32:
        case INTGR64: {
          // Check for overflow past 64 bits
          int64_t left  = node->left.value.asInt64;
          int64_t right = node->right.value.asInt64;

          int64_t diff  = left-right;
          if (DIFF_SIGN(left, right) && DIFF_SIGN(left, diff)) {
            *report = ERR_OPP_INT_OVERFLOW(node->oprtrPos, node->oprtrPos+1);
            return false;
          }

          // Set data type to smallest possible size
          node->left.dataType = _find_int_type(node, diff);

          return true;
        }
        case FLT:
        case DBL: {
          // Check for overflow past double
          double diff = node->left.value.asInt64-node->right.value.asDouble;
          if (IS_DBL_INF(diff)) {
            *report = ERR_OPP_DBL_OVERFLOW(node->oprtrPos, node->oprtrPos+1);
            return false;
          }

          // Set data type to smallest possible size
          node->left.dataType = _find_flt_type(node, diff);

          return true;
        }
        default: {
          break;
        }
      }
      break;
    }
    case FLT:
    case DBL: {
      switch (node->right.dataType) {
        case INTGR32:
        case INTGR64: {
          // Check for overflow past double
          double diff = node->left.value.asDouble-node->right.value.asInt64;
          if (IS_DBL_INF(diff)) {
            *report = ERR_OPP_DBL_OVERFLOW(node->oprtrPos, node->oprtrPos+1);
            return false;
          }

          // Set data type to smallest possible size
          node->left.dataType = _find_flt_type(node, diff);

          return true;
        }
        case FLT:
        case DBL: {
          // Check for overflow past double
          double diff = node->left.value.asDouble-node->right.value.asDouble;
          if (IS_DBL_INF(diff)) {
            *report = ERR_OPP_DBL_OVERFLOW(node->oprtrPos, node->oprtrPos+1);
            return false;
          }

          // Set data type to smallest possible size
          node->left.dataType = _find_flt_type(node, diff);

          return true;
        }
        default: {
          break;
        }
      }
      break;
    }
    default: {
      break;
    }
  }
  return false;
}

static bool
_and(ASTNode* node, ErrorReport* report) {
  switch (node->left.dataType) {
    case INTGR32:
    case INTGR64: {
      switch (node->right.dataType) {
        case INTGR32:
        case INTGR64: {
          // Set data type to smallest possible size
          node->left.dataType = _find_int_type(node,
            node->left.value.asInt64&node->right.value.asInt64);

          return true;
        }
        default: {
          break;
        }
      }
      break;
    }
    default: {
      break;
    }
  }

  *report = ERR_BIT_FLOAT(node->oprtrPos, node->oprtrPos+1);
  return false;
}

static bool
_or(ASTNode* node, ErrorReport* report) {
  switch (node->left.dataType) {
    case INTGR32:
    case INTGR64: {
      switch (node->right.dataType) {
        case INTGR32:
        case INTGR64: {
          // Set data type to smallest possible size
          node->left.dataType = _find_int_type(node,
            node->left.value.asInt64|node->right.value.asInt64);

          return true;
        }
        default: {
          break;
        }
      }
      break;
    }
    default: {
      break;
    }
  }

  *report = ERR_BIT_FLOAT(node->oprtrPos, node->oprtrPos+1);
  return false;
}

static bool
_xor(ASTNode* node, ErrorReport* report) {
  switch (node->left.dataType) {
    case INTGR32:
    case INTGR64: {
      switch (node->right.dataType) {
        case INTGR32:
        case INTGR64: {
          // Set data type to smallest possible size
          node->left.dataType = _find_int_type(node,
            node->left.value.asInt64^node->right.value.asInt64);

          return true;
        }
        default: {
          break;
        }
      }
      break;
    }
    default: {
      break;
    }
  }

  *report = ERR_BIT_FLOAT(node->oprtrPos, node->oprtrPos+1);
  return false;
}

/* Functions for removing branches from abstract syntax tree */
static void
_mutate_branch(ASTBranch* branch) {
  switch (branch->dataType) {
    case INTGR32:
      // Mutate 32 bit integers
      if (IS_BOOL(branch->mutation)) {
        branch->value.asInt32 = branch->value.asInt32 ? 1 : 0;
      } else if (IS_NOT(branch->mutation)) {
        branch->value.asInt32 = !branch->value.asInt32;
      }
      if (IS_INV(branch->mutation)) {
        branch->value.asInt32 = ~branch->value.asInt32;
      }
      CLEAR_MUTS(branch->mutation);
      break;

    case INTGR64:
      // Mutate 64 bit integers
      if (IS_BOOL(branch->mutation)) {
        branch->value.asInt64 = branch->value.asInt64 ? 1 : 0;
      } else if (IS_NOT(branch->mutation)) {
        branch->value.asInt64 = !branch->value.asInt64;
      }
      if (IS_INV(branch->mutation)) {
        branch->value.asInt64 = ~branch->value.asInt64;
      }
      CLEAR_MUTS(branch->mutation);
      break;

    case FLT:
      // Mutate floats
      if (IS_BOOL(branch->mutation)) {
        branch->value.asFloat = branch->value.asFloat ? 1 : 0;
      } else if (IS_NOT(branch->mutation)) {
        branch->value.asFloat = !branch->value.asFloat;
      }
      CLEAR_MUTS(branch->mutation);
      break;

    case DBL:
      // Mutate doubles
      if (IS_BOOL(branch->mutation)) {
        branch->value.asDouble = branch->value.asDouble ? 1 : 0;
      } else if (IS_NOT(branch->mutation)) {
        branch->value.asDouble = !branch->value.asDouble;
      }
      CLEAR_MUTS(branch->mutation);
      break;

    default:
      break;
  }

  return;
}

static bool
_simplify(ASTNode* node, ErrorReport* report) {
  _mutate_branch(&node->left);
  _mutate_branch(&node->right);
  switch (node->oprtr) {
    case MLT:
      return _mlt(node, report);
    case DIV:
      return _div(node, report);
    case ADD:
      return _add(node, report);
    case SUB:
      return _sub(node, report);
    case AND:
      return _and(node, report);
    case OR:
      return _or(node, report);
    case XOR:
      return _xor(node, report);
    default:
      break;
  }

  return false;
}

static ASTNode*
_traverse_to_bottom(ASTNode* node) {
  // Traverse to non-visited nodes
  while (((node->left.dataType == BRNCH) && node->left.branch->isNotVisited)
  || ((node->right.dataType == BRNCH) && node->right.branch->isNotVisited)) {
    node = ((node->left.dataType == BRNCH) &&
      (node->left.branch->isNotVisited)) ?
      node->left.branch : node->right.branch;
  }

  return node;
}

static void
_simplify_subtree(ASTNode* node, ErrorReport* report) {
  for (node = _traverse_to_bottom(node); !report->isError &&
  node->prev != NULL; node = _traverse_to_bottom(node)) {
    if (_simplify(node, report)) {
      // Save simplified value
      if (node->prev->left.dataType == BRNCH &&
      node->prev->left.branch == node) {
        node->prev->left  = node->left;
      } else {
        node->prev->right = node->left;
      }

      // Free simplified node
      ASTNode* temp = node;
      node = node->prev;
      free(temp);
    } else {
      // Set node as visited
      node->isNotVisited  = false;
      node                = node->prev;
    }
  }

  return;
}

void
simplify_ast(AST ast, ErrorReport* report) {
  // Check head node has value
  if (ast.head == NULL) {
    return;
  }

  // Simplify left subtree
  if (ast.head->left.dataType == BRNCH) {
    _simplify_subtree(ast.head->left.branch, report);
  }

  // Simplify right subtree
  if (!report->isError && ast.head->right.dataType == BRNCH) {
    _simplify_subtree(ast.head->right.branch, report);
  }

  // Simplify entire tree to a value
  if (!report->isError && _simplify(ast.head, report)) {
    ast.head->oprtr = NOP;
  }

  // WRITE A FUNCTION TO CONDENSE ANY ZERO VALUES INTO AN INT32, AND OTHER HARD-CODED NUMBERS TO THEIR SMALLEST SIZE (DOUBLES TO FLOATS)
  // _COMPRESS_NUMBERS(AST);

  // ALSO, MAYBE DO SOME CHECKING ABOUT PERCISSION??? FLOATS AND DOUBLES ARE TRICKY

  return;
}
