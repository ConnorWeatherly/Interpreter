#include <stdio.h>
#include <stdlib.h>
#include "priority.h"

static SymbolKey _parse_parenthesis(ParseNode* head, char** str) {
  return CLS_PAR;
}
static bool _node_less_priority(ParseNode* node, Symbol symbol) {
  return node->operator < symbol.operator;
}

ParseNode* parse_expression(char* str, Error* error) {
  char* pos = str;

  // Assume no error occurs
  *error = (Error) {
    .isError = false,
  };

  // Set the first node as empty
  ParseNode* node = malloc(sizeof(*node));
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
      // free(node);
      *error = (Error) {
        .isError = true,
        .message = "no expression to parse",
        .startIdx = 0,
        .endIdx = 1,
      };
      return NULL;
    case OPRTR:
      free(node);
      *error = (Error) {
        .isError = true,
        .message = "illegal operator at start of expression",
        .startIdx = pos-str-1,
        .endIdx = pos-str,
      };
      return NULL;
    case CLS_PAR:
      free(node);
      *error = (Error) {
        .isError = true,
        .message = "illegal close brace with no opening argument",
        .startIdx = pos-str-1,
        .endIdx = pos-str,
      };
      return NULL;
    case OPN_PAR:
      node->leftKey = _parse_parenthesis(node, &pos);
      if (node->leftKey == NONE) {
        free(node);
        *error = (Error) {
          .isError = true,
          .message = "illegal open brace with no closing argument",
          .startIdx = pos-str-1,
          .endIdx = pos-str,
        };
        return NULL;
      }
    default:
      break;
  }

  // Ensure the next symbol is an operator
  Symbol symbol;
  SymbolKey key = parse_symbol(&pos, &symbol);
  switch (key) {
    case NONE:
      // Only one symbol in parsed expression
      free(node);
      return NULL;
    case OPRTR:
      // Operator follows parsed symbol
      break;
    default:
      free(node);
      *error = (Error) {
        .isError = true,
        .message = "double evaluable argument with no operator",
        .startIdx = pos-str-1,
        .endIdx = pos-str,
      };
      return NULL;
  }

  // Convert subtraction to negative addition
  bool isNeg = false;
  if (symbol.operator == SUB) {
    symbol.operator = ADD;
    isNeg = true;
  }
  node->operator = symbol.operator;

  // Read in the rest of the values
  for (key = parse_symbol(&pos, &symbol);
  key != NONE; key = parse_symbol(&pos, &symbol)) {
    if (key == OPRTR) {
      // Validate the expression has correct syntax
      if (node->rightKey == NONE && node->right == NULL) {
        free_expression(node);
        *error = (Error) {
          .isError = true,
          .message = "double operator with no evaluable argument",
          .startIdx = pos-str-1,
          .endIdx = pos-str,
        };
        return NULL;
      }

      // Convert subtraction to negative addition
      isNeg = false;
      if (symbol.operator == SUB) {
        symbol.operator = ADD;
        isNeg = true;
      }

      // Traverse to correct priority node
      while (node->prev != NULL && _node_less_priority(node, symbol)) {
        node = node->prev;
      }

      // Make a new head node
      if (_node_less_priority(node, symbol)) {
        ParseNode* head = malloc(sizeof(*head));
        *head = (ParseNode) {
          .operator = symbol.operator,
          .prev     = NULL,
          .leftKey  = NONE,
          .left     = node,
          .rightKey = NONE,
          .right    = NULL,
        };

        node->prev = head;
        node = head;
      } else {
        // Create a new tree node
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
      }
    } else {
      // Convert the symbol to the correct sign
      if (isNeg) {
        switch (key) {
          case FLT:
            symbol.floating = -symbol.floating;
            break;
          case INT:
            symbol.integer = -symbol.integer;
            break;
          case VAR:
            // Set flag in variable to negative
            break;
          default:
            break;
        }
      }

      // Store the new value in the brance node
      node->rightKey    = key;
      node->rightSymbol = symbol;
    }
  }

  // Find the head of the tree for returning
  while (node->prev != NULL) {
    node = node->prev;
  }

  return node;
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
  uint32_t print_end = error.endIdx-1;
  while (str[print_end] != '\n' && str[print_end] != '\0') {
    print_end += 1;
  }

  // Print the error report
  fprintf(stderr, TEXT_BOLD "line %u: "
  TEXT_RED "error: " TEXT_BOLD, lineNum);
  fputs(error.message, stderr);

  // Exit before printing error line if no text present
  if (print_start == print_end) {
    fprintf(stderr, TEXT_RESET "\n");
    return;
  }

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
