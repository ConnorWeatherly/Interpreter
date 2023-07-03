#ifndef __OPERATORS_H__
#define __OPERATORS_H__

#include "priority.h"

typedef struct {
  SymbolKey key;
  Symbol    symbol;
} SymbolNode;

// NOT IS AN XOR WITH ALL ONES OF THE CORRECT TYPE

SymbolNode mlt_fn(ParseNode* node);
SymbolNode div_fn(ParseNode* node);
SymbolNode add_fn(ParseNode* node);
SymbolNode and_fn(ParseNode* node);
SymbolNode or_fn(ParseNode* node);
SymbolNode xor_fn(ParseNode* node);
// SymbolNode not_fn(SymbolKey type, Node* node);

#endif
