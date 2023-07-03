#ifndef __PARSER_H__
#define __PARSER_H__

#include <stdbool.h>

// Table to map each symbol
typedef enum {
  NONE, OPN_PAR, CLS_PAR, OPRTR, FLT, INT, VAR,
} SymbolKey;

// Table of parsable symbols from highest to lowest priority
typedef enum {
  NOP, MLT, DIV, ADD, SUB, AND, OR, XOR, // NOT,
/*   , *  , /  , +  , -  , &  , | , ^  , ~  */
} Operation;

// Structure wrapped to hold type union
typedef struct {
  union {
    Operation operator;
    float     floating;
    int       integer;
    char*     variable;
  };
} Symbol;

// Read space symbols
bool is_space(char ch);

// Function for parsing all symbols
SymbolKey parse_symbol(char** pos, Symbol* symbol);

#endif
