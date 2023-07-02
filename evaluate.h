#ifndef __EVALUATE_H__
#define __EVALUATE_H__

#include "priority.h"

typedef struct {
  SymbolKey key;
  Symbol    symbol;
} OperationNode;

typedef struct {
  uint64_t        size;
  OperationNode*  operations;
} Evaluation;

Evaluation evaluate(ParseNode* head);

#endif
