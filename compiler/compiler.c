#include "front_end/ast_structs.h"
#include "front_end/error_report.h"
#include "front_end/build_ast.h"
#include "front_end/simplify_ast.h"
#include "front_end/parser.h"

#include <stdlib.h>
#include <stdio.h>

#include <float.h>

static void _print_bits(void* numAddr) {
  uint64_t num = *(uint64_t*)numAddr;

  for (uint64_t mask = (uint64_t)1<<63; mask != 0x00; mask >>= 1) {
    fputc((mask&num) ? '1' : '0', stderr);
  }
  fputc('\n', stderr);
  return;
}

static void _print_mutation(Mutation mutation) {
  if (IS_NEG(mutation)) {
    fputc('-', stderr);
  }
  if (IS_NOT(mutation)) {
    fputc('!', stderr);
  }
  if (IS_BOOL(mutation)) {
    fputc('!', stderr);
  }
  if (IS_INV(mutation)) {
    fputc('~', stderr);
  }
  return;
}
static void _print_value(ASTBranch branch) {
  switch (branch.dataType) {
    case INTGR32:
      fprintf(stderr, "INT32: %d", branch.value.asInt32);
      break;
    case INTGR64:
      fprintf(stderr, "INT64: %lld", branch.value.asInt64);
      break;
    case FLT:
      fprintf(stderr, "FLT: %f", branch.value.asFloat);
      break;
    case DBL:
      fprintf(stderr, "DBL: %lf", branch.value.asDouble);
      break;
    case VAR:
      _print_mutation(branch.mutation);
      fputs(branch.value.asVariable, stderr);
    default:
      fprintf(stderr, "???");
  }

  return;
}
static void _print_operator(Operator oprtr) {
  switch (oprtr) {
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
    case SEMICOLON:
      fputc(';', stderr);
      break;
    default:
      fputc('?', stderr);
      break;
  }

  return;
}
static void _validate_tree(ASTNode* head) {
  if (head == NULL) {
    fprintf(stderr, "NULL node\n");
    return;
  }

  if (head->left.dataType == BRNCH) {
    if (head->left.branch->prev != head) {
      fprintf(stderr, "left connection incorrect\n");
    }
    _validate_tree(head->left.branch);
  } else {
    // fprintf(stderr, "left: ");
    // _print_value(head->left);
    // fputc('\n', stderr);
  }
  if (head->right.dataType == BRNCH) {
    if (head->right.branch->prev != head) {
      fprintf(stderr, "right connection incorrect\n");
    }
    _validate_tree(head->right.branch);
  } else {
    // fprintf(stderr, "right: ");
    // _print_value(head->right);
    // fputc('\n', stderr);
  }

  return;
}
static char* _read_file(char* fileName) {
  FILE* file = fopen(fileName, "r");

  if (file == NULL) {
    fprintf(stderr, "file [%s] does not exist\n", fileName);
    return NULL;
  }

  fseek(file, 0L, SEEK_END);
  long int size = ftell(file);
  fseek(file, 0L, SEEK_SET);

  char* content = malloc(sizeof(*content)*size);
  fread(content, sizeof(*content), size, file);

  if (fclose(file)) {
    free(content);
    return NULL;
  }

  return content;
}

int main(int argc, char** argv) {
  char* str;
  if (argc != 2) {
    str = _read_file("input.txt");
    if (str == NULL) {
      return EXIT_FAILURE;
    }
  } else {
    str = _read_file(argv[1]);
    if (str == NULL) {
      return EXIT_FAILURE;
    }
  }

  if (*str == '\0') {
    fprintf(stderr, "Empty file\n");
    free(str);
    return EXIT_SUCCESS;
  }

  // Read line from file
  ErrorReport report;
  AST ast = build_line_ast(str, &report);
  if (is_error(str, report)) {
    free(str);
    return EXIT_FAILURE;
  }

  _validate_tree(ast.head);

  simplify_ast(ast, &report);
  if (is_error(str, report)) {
    free(str);
    return EXIT_FAILURE;
  }

  _validate_tree(ast.head);
  
  fputs(str, stderr);
  _print_value(ast.head->left);
  fputc('\n', stderr);

  free_ast(&ast);

  free(str);

  return EXIT_SUCCESS;
}
