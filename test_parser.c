#include <stdio.h>
#include <stdlib.h>
#include "priority.h"
#include "evaluate.h"

static void _print_operator(Operation operator) {
  switch (operator) {
    case NOP:
      break;
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
    // case NOT:
    //   fputc('~', stderr);
    //   break;
    default:
      fprintf(stderr, "Error: Invalid operator\n");
  }
  return;
}
static void _print_symbol(SymbolKey key, Symbol symbol) {
  switch (key) {
    case NONE:
      break;
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
static void _print_eval(Evaluation eval) {
  OperationNode* head = eval.operations;
  while (head->isLast == false) {
    _print_symbol(head->value.key, head->value.symbol);
    _print_operator(head->operator);

    head = head->next;
  }

  _print_symbol(head->value.key, head->value.symbol);
  _print_operator(head->operator);
  _print_symbol(head->endValue.key, head->endValue.symbol);

  fputc('\n', stderr);

  return;
}

int main(int argc, char** argv) {
  char* str = "7 + 3 ^ 9 * 11";

  Error error;
  ParseNode* head = parse_expression(str, &error);

  if (error.isError) {
    print_error_report(str, error);
  } else {
    _print_expression(*head);
    Evaluation evaluation = evaluate(head);
    if (evaluation.isError) {
      fputs(evaluation.message, stderr);
      fputc('\n', stderr);
    } else {
      _print_eval(evaluation);
    }
  }

  return EXIT_SUCCESS;
}
