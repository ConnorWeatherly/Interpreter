#ifndef __EVALUATE_H__
#define __EVALUATE_H__

#include "priority.h"
#include "operators.h"

// Linked list of nodes to evaluate in order
typedef struct _OperationNode {
  SymbolNode  value;
  Operation   operator;

  bool        isLast;
  union {
    SymbolNode              endValue;
    struct _OperationNode*  next;
  };
} OperationNode;

// Operations lists ends when operator is NONE
typedef struct {
  bool            isError;
  union {
    const char*     message;
    OperationNode*  operations;
  };
} Evaluation;

// Create an array out of the priority tree
Evaluation evaluate(ParseNode* head);

#endif
