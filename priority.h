#ifndef __PRIORITY_H__
#define __PRIORITY_H__

#include <stdint.h>
#include "parser.h"

// Tree used for parsing priorities
typedef struct _ParseNode {
  Operation           operator;
  struct _ParseNode*  prev;

  SymbolKey leftKey;
  union {
    struct _ParseNode*  left;
    Symbol              leftSymbol;
  };

  SymbolKey rightKey;
  union {
    struct _ParseNode*  right;
    Symbol              rightSymbol;
  };
} ParseNode;

// Structure to hold details of error
typedef struct {
  bool        isError;
  const char* message;
  uint32_t    startIdx;
  uint32_t    endIdx;
} Error;

// Text colors from printing error messages
#define TEXT_GREEN  "\x1b[32;1m"
#define TEXT_RED    "\x1b[31;1m"
#define TEXT_BOLD   "\x1b[0;1m"
#define TEXT_RESET  "\x1b[0m"

// Parse expressions from start to end
ParseNode* parse_expression(char* str, Error* error);

// Display details of parsing error
void print_error_report(char* str, Error error);

// Remove data malloced while parsing
void free_expression(ParseNode* head);

#endif
