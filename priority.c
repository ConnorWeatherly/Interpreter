#include <stdio.h>
#include <stdlib.h>
#include "priority.h"
#include "errors.h"

static bool _parse_braces(char* str, char** pos, ParseNode* head) {
  // node->leftKey = _parse_what_you_need_to_parse_here();
  return CLS_PAR;
}
// static bool _node_less_priority(ParseNode* node, Symbol symbol) {
//   // Make operations with the same priority equal
//   Operation nodeOperation = node->operator;
//   switch (nodeOperation) {
//     case DIV:
//       nodeOperation = MLT;
//       break;
//     case SUB:
//       nodeOperation = ADD;
//       break;
//     case XOR:
//       nodeOperation = OR;
//       break;
//     default:
//       break;
//   }
//   switch (symbol.operator) {
//     case DIV:
//       symbol.operator = MLT;
//       break;
//     case SUB:
//       symbol.operator = ADD;
//       break;
//     case XOR:
//       symbol.operator = OR;
//       break;
//     default:
//       break;
//   }
//   return nodeOperation <= symbol.operator;
// }
static bool _node_less_priority(ParseNode* node, Operation operator) {
  // Make operations with the same priority equal
  Operation nodeOperation = node->operator;
  switch (nodeOperation) {
    case DIV:
      nodeOperation = MLT;
      break;
    case SUB:
      nodeOperation = ADD;
      break;
    case XOR:
      nodeOperation = OR;
      break;
    default:
      break;
  }
  switch (operator) {
    case DIV:
      operator = MLT;
      break;
    case SUB:
      operator = ADD;
      break;
    case XOR:
      operator = OR;
      break;
    default:
      break;
  }
  return nodeOperation <= operator;
}
static bool _parse_leading_neg(char** pos, ParseNode* node) {
  // Check for leading negative sign
  if (node->leftSymbol.operator != SUB) {
    return false;
  }

  // Read for valid negative integer
  if (parse_integer(pos, &node->leftSymbol.integer)) {
    node->leftSymbol.integer = -node->leftSymbol.integer;
    node->leftKey = INT;
    return true;
  }

  // Read for valid negative float
  if (parse_float(pos, &node->leftSymbol.floating)) {
    node->leftSymbol.floating = -node->leftSymbol.floating;
    node->leftKey = FLT;
    return true;
  }

  // Illegal start of expression
  return false;
}

static bool _make_right_value_neg(ParseNode* node) {
  switch (node->rightKey) {
    case INT:
      node->rightSymbol.integer = -node->rightSymbol.integer;
      break;
    case FLT:
      node->rightSymbol.floating = -node->rightSymbol.floating;
      break;
    default:
      return false;
  }

  return true;
}

static bool _validate_left_symbol(char* str, char** pos,
ParseNode* node, Error* error) {
  switch (node->leftKey) {
    case NONE:
      // No first symbol
      *error = ERR_NO_EXPRESSION(str, *pos);
      return false;
    case OPRTR:
      // Check for nagative number
      if (!_parse_leading_neg(pos, node)) {
        *error = ERR_ILLEGAL_START(str, *pos);
        return false;
      }
      break;
    case CLS_PAR:
      // First argument cannot be a close brace
      *error = ERR_ILLEGAL_CLOSE_BRACE(str, *pos);
      return false;
    case OPN_PAR:
      // Check for valid expression in braces
      if (!_parse_braces(str, pos, node)) {
        *error = ERR_ILLEGAL_OPEN_BRACE(str, *pos);
        return false;
      }
    default:
      break;
  }

  return true;
}
static bool _validate_right_symbol(char* str, char** pos, ParseNode* node,
bool isNeg, Error* error) {
  switch (node->rightKey) {
    case NONE:
      // No first symbol
      *error = ERR_END_SYMBOL(str, *pos);
      return false;
    case OPRTR:
      // Check for nagative number
      if (!_parse_leading_neg(pos, node)) {
        *error = ERR_DOUBLE_OPERATOR(str, *pos);
        return false;
      }
      break;
    case CLS_PAR:
      // First argument cannot be a close brace
      *error = ERR_ILLEGAL_CLOSE_BRACE(str, *pos);
      return false;
    case OPN_PAR:
      // Check for valid expression in braces
      if (!_parse_braces(str, pos, node)) {
        *error = ERR_ILLEGAL_OPEN_BRACE(str, *pos);
        return false;
      }
    default:
      break;
  }

  // Flip the value to negative if possible
  if (isNeg && !_make_right_value_neg(node)) {
    *error = ERR_ILLEGAL_ARGUMENT(str, *pos);
    return false;
  }

  return true;
}
static bool _validate_operator(char* str, char** pos,
SymbolKey key, Error* error) {
  switch (key) {
    case NONE:
    case OPRTR:
      return true;
    default:
      *error = ERR_DOUBLE_SYMBOL(str, *pos);
      break;
  }

  return false;
}

static ParseNode* _insert_node(ParseNode* node, Operation operator) {
  // Traverse to correct priority node
  while (node->prev != NULL && _node_less_priority(node, operator)) {
    node = node->prev;
  }

  // Make a new head node
  if (_node_less_priority(node, operator)) {
    ParseNode* head = malloc(sizeof(*head));
    *head = (ParseNode) {
      .operator = operator,
      .prev     = NULL,
      .leftKey  = NONE,
      .left     = node,
      .rightKey = NONE,
      .right    = NULL,
    };

    node->prev = head;
    return head;
  }

  // Copy all data from right into temp node
  ParseNode* temp = node->right;
  node->right = malloc(sizeof(*node));

  // Build the new tree node
  *node->right = (ParseNode) {
    .operator = operator,
    .prev     = node,
    .leftKey  = node->rightKey,
    .left     = temp,
    .rightKey = NONE,
    .right    = NULL,
  };

  if (node->rightKey == NONE) {
    temp->prev = node->right;
  }

  // Adjust the old node
  node->rightKey = NONE;
  node = node->right;

  return node;
}

ParseNode* parse_expression(char* str, Error* error) {
  char* pos = str;

  // Assume no error occurs
  *error = NO_ERR;

  // Set the first node as empty
  ParseNode* node = malloc(sizeof(*node));
  *node = (ParseNode) {
    .operator = NOP,
    .prev     = NULL,
    .leftKey  = NONE,
    .left     = NULL,
    .rightKey = NONE,
    .right    = NULL,
  };

  // Read and evaluate the first expression
  node->leftKey = parse_symbol(&pos, &node->leftSymbol);
  if (!_validate_left_symbol(str, &pos, node, error)) {
    free_expression(node);
    return NULL;
  }

  // Ensure the next symbol is an operator
  SymbolKey key = parse_symbol(&pos, (Symbol*)(&node->operator));
  if (!_validate_operator(str, &pos, key, error)) {
    free_expression(node);
    return NULL;
  }

  // Convert subtraction to negative addition
  bool isNeg = (node->operator == SUB);
  node->operator = isNeg ? ADD : node->operator;

  // Read in second symbol
  node->rightKey = parse_symbol(&pos, &node->rightSymbol);
  if (!_validate_right_symbol(str, &pos, node, isNeg, error)) {
    free_expression(node);
    return NULL;
  }

  // Loop over all arguments in expression
  Operation operator = NOP;
  for (key = parse_symbol(&pos, (Symbol*)(&operator));
  key != NONE && _validate_operator(str, &pos, key, error);
  key = parse_symbol(&pos, (Symbol*)(&operator))) {
    // Convert subtraction to negative addition
    isNeg = (operator == SUB);
    operator = isNeg ? ADD : operator;

    // Insert new node with operator
    node = _insert_node(node, operator);

    // Read in next symbol
    node->rightKey = parse_symbol(&pos, &node->rightSymbol);
    if (!_validate_right_symbol(str, &pos, node, isNeg, error)) {
      free_expression(node);
      return NULL;
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
