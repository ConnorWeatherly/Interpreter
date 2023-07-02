#include <stdio.h>
#include <stdlib.h>
#include "priority.h"

static SymbolKey _parse_parenthesis(ParseNode* head, char** str) {
  return CLS_PAR;
}
static bool _node_less_priority(ParseNode* node, SymbolKey key, Symbol symbol) {
  return (key != NONE) && (node->operator < symbol.operator);
}

Error parse_expression(ParseNode* node, char* str) {
  char* pos = str;

  // Set the first node as empty
  *node = (ParseNode) {
    .operator = NOP,
    .prev     = NULL,
    .left     = NULL,
    .leftKey  = NONE,
    .rightKey = NONE,
    .right    = NULL,
  };

  // Read and evaluate the first expression
  node->leftKey = parse_symbol(&pos, &node->leftSymbol);
  switch (node->leftKey) {
    case NONE:
      return (Error) {.isError = false};
    case OPRTR:
      return (Error) {
        .isError = true,
        .message = "illegal operator at start of expression",
        .startIdx = pos-str-1,
        .endIdx = pos-str,
      };
    case CLS_PAR:
      return (Error) {
        .isError = true,
        .message = "illegal close brace with no opening argument",
        .startIdx = pos-str-1,
        .endIdx = pos-str,
      };
    case OPN_PAR:
      node->leftKey = _parse_parenthesis(node, &pos);
      if (node->leftKey == NONE) {
        return (Error) {
          .isError = true,
          .message = "illegal open brace with no closing argument",
          .startIdx = pos-str-1,
          .endIdx = pos-str,
        };
      }
    default:
      break;
  }

  Symbol symbol;
  SymbolKey key = parse_symbol(&pos, &symbol);
  switch (key) {
    case NONE:
      // Only one symbol in parsed expression
      return (Error) {.isError = false};
    case OPRTR:
      // Operator follows parsed symbol
      break;
    default:
      return (Error) {
        .isError = true,
        .message = "double evaluable argument with no operator",
        .startIdx = pos-str-1,
        .endIdx = pos-str,
      };
  }
  node->operator = symbol.operator;

/* THIS LOOP IS NOT CORRECT
  You need to make sure you malloc a node while making a new operator
  You have to not malloc a node while inserting a new value
*/

  // Read in the rest of the values
  for (key = parse_symbol(&pos, &symbol);
  key != NONE; key = parse_symbol(&pos, &symbol)) {
    if (key == OPRTR) {
      // Validate the expression has correct syntax
      if (node->rightKey == NONE && node->right == NULL) {
        return (Error) {
          .isError = true,
          .message = "double operator with no evaluable argument",
          .startIdx = pos-str-1,
          .endIdx = pos-str,
        };
      }

      // Traverse to correct priority node
      while (node->prev != NULL && _node_less_priority(node, key, symbol)) {
        node = node->prev;
      }

      // Create a new node
      Symbol temp = node->rightSymbol;
      node->right = malloc(sizeof(*node));

      // Build the new tree node
      *node->right = (ParseNode) {
        .operator   = symbol.operator,
        .prev       = node,
        .leftKey    = node->rightKey,
        .leftSymbol = temp,
        .rightKey   = NONE,
        .right      = NULL,
      };

      // Adjust the old node
      node->rightKey = NONE;
      node = node->right;
    } else {
      // Store the new value in the brance node
      node->rightKey    = key;
      node->rightSymbol = symbol;
    }
  }

  // Find the head of the tree for returning
  while (node->prev != NULL) {
    node = node->prev;
  }

  return (Error) {.isError = false};

/* Expression to evaluate */
  // 1 & 3 + 5 * 7

/* IN ORDER */
  // (1 & 3) + 5 * 7
  //   AND
  //  /  \
  // 1   3

  // 1 & (3 + 5) * 7
  //   AND
  //  /   \
  // 1   ADD
  //    /   \
  //   3    5

  // 1 & 3 + (5 * 7)
  //   AND
  //  /   \
  // 1   ADD
  //    /   \
  //   3   MULT
  //      /   \
  //     5    7

/* REVERSE ORDER */
  // (7 * 5) + 3 & 1
  //   MULT
  //  /   \
  // 7    5

  // 7 * (5 + 3) & 1
  //   ADD
  //  /  \
  // 5  MULT
  //   /   \
  //  7    5

  // 7 * 5 + (3 & 1)
  //   AND
  //  /   \
  // 1   ADD
  //    /   \
  //   3   MULT
  //      /   \
  //     5    7

/* MIX ORDER */
  //  (3 + 7) * 5 & 1
  //   ADD
  //  /  \
  // 3   7

  // 3 + (7 * 5) & 1
  //   ADD
  //  /  \
  // 3  MULT
  //   /   \
  //  7    5

  // 7 * 5 + (3 & 1)
  //     AND
  //    /   \
  //   ADD  1
  //  /   \
  // 3   MULT
  //    /   \
  //   7    5
}

void print_error_report(char* str, Error error) {
  // Track the line the error occured on
  uint32_t lineNum = 1;
  for (uint32_t i = 0; i < error.startIdx; i += 1) {
    lineNum += (str[i] == '\n');
  }

  // Find the start of the current line
  uint32_t print_start = 0;
  if (lineNum > 1) {
    print_start = error.startIdx;
    while (str[print_start] != '\n') {
      print_start -= 1;
    }
  }

  // Remove leading spaces from line start
  while (is_space(str[print_start])) {
    print_start += 1;
  }

  // Find the end of the current line
  uint32_t print_end = error.endIdx+1;
  while (str[print_end] != '\n' && str[print_end] != '\0') {
    print_end += 1;
  }

  // Print the error report
  fprintf(stderr, TEXT_BOLD "line %u: "
  TEXT_RED "error: " TEXT_BOLD, lineNum);
  fputs(error.message, stderr);

  // Print the line with the error
  fprintf(stderr, TEXT_RESET "\n  ");
  for (uint32_t i = print_start; i < print_end; i += 1) {
    fputc(str[i], stderr);
  }

  // Print where the error is
  fprintf(stderr, "\n  " TEXT_GREEN);
  for (uint32_t i = print_start; i < error.startIdx; i += 1) {
    fputc(' ', stderr);
  }
  for (uint32_t i = error.startIdx; i < error.endIdx; i += 1) {
    fputc('^', stderr);
  }
  fprintf(stderr, TEXT_RESET "\n");

  return;
}
void free_expression(ParseNode* head) {
  // FREE THE TREE BY TRAVERING DOWN AND BACK UP USING PREV
  return;
}
