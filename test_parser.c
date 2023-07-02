#include <stdio.h>
#include <stdlib.h>
#include "priority.h"
#include "evaluate.h"

static void _print_operator(Operation operator) {
  switch (operator) {
    case MLT:
      fputc('*', stderr);
      break;
    case DIV:
      fputc('/', stderr);
      break;
    case ADD:
      fputc('+', stderr);
      break;
    case SUB:
      fputc('-', stderr);
      break;
    case AND:
      fputc('&', stderr);
      break;
    case OR:
      fputc('|', stderr);
      break;
    case XOR:
      fputc('^', stderr);
      break;
    case NOT:
      fputc('~', stderr);
      break;
    default:
      fprintf(stderr, "Error: Invalid operator\n");
  }
  return;
}
static void _print_symbol(SymbolKey key, Symbol symbol) {
  switch (key) {
    case OPRTR:
      _print_operator(symbol.operator);
      break;
    case FLT:
      fprintf(stderr, "%f", symbol.floating);
      break;
    case INT:
      fprintf(stderr, "%d", symbol.integer);
      break;
    case VAR:
      fputs(symbol.variable, stderr);
      break;
    case NONE:
      fprintf(stderr, "\n\nError: Type not set\n\n");
      break;
    default:
      fprintf(stderr, "\n\nError: Unknown type\n\n");
  }
  return;
}
static void _print_tree_recursive(ParseNode* node) {
  if (node == NULL) {
    return;
  }

  if (node->leftKey == NONE) {
    _print_tree_recursive(node->left);
  } else {
    _print_symbol(node->leftKey, node->leftSymbol);
  }

  if (node->operator != NOP) {
    _print_symbol(OPRTR, (Symbol) {.operator = node->operator});
  }

  if (node->rightKey == NONE) {
    _print_tree_recursive(node->right);
  } else {
    _print_symbol(node->rightKey, node->rightSymbol);
  }

  return;
}
static void _print_expression(ParseNode head) {
  ParseNode* node = &head;
  _print_tree_recursive(node);
  fputc('\n', stderr);
  return;
}

int main(int argc, char** argv) {
  char* str = "3 + 7 - 8 * 9";

  // MAKE IT SUCH THAT YOU PASS ERROR AND RETURN HEAD
  ParseNode head;
  Error error = parse_expression(&head, str);
  if (error.isError) {
    print_error_report(str, error);
  } else {
    _print_expression(head);
    Evaluation evaluation = evaluate(&head);
  }

  return EXIT_SUCCESS;
}
