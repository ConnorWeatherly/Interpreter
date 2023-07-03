#include <stdlib.h>
#include "evaluate.h"

static bool _can_traverse_left(ParseNode* node) {
  return (node->leftKey == NONE) && (node->left != NULL);
}
static bool _can_traverse_right(ParseNode* node) {
  return (node->rightKey == NONE) && (node->right != NULL);
}
static bool _can_traverse(ParseNode* node) {
  return _can_traverse_left(node) || _can_traverse_right(node);
}
static void _insert_node(OperationNode** node) {
  return;
}
static bool _can_simplify(ParseNode* node) {
  return (node->leftKey != VAR) && (node->rightKey != VAR);
}
static SymbolNode _simplify(ParseNode* node) {
  // Simplify with the given operation
  switch (node->operator) {
    case MLT:
      return mlt_fn(node);
    case DIV:
      return div_fn(node);
    case ADD:
      return add_fn(node);
    case AND:
      return and_fn(node);
    case OR:
      return or_fn(node);
    case XOR:
      return xor_fn(node);
    default:
      break;
  }

  // No valid expression
  return (SymbolNode) {
    .key    = NONE,
    .symbol = (Symbol) {
      .operator = NOP,
    },
  };
}

Evaluation evaluate(ParseNode* head) {
  // Take care of the empty case
  if (head == NULL) {
    return (Evaluation) {
      .isError = true,
      .message = "no arguments to evaluate",
    };
  }

  // Traverse to bottom node
  while (_can_traverse(head) || (_can_simplify(head) && head->prev != NULL)) {
    if (_can_traverse_left(head)) {
      // Traverse to and clear left node
      head = head->left;
      head->prev->left = NULL;
    } else if (_can_traverse_right(head)) {
      // Traverse to and clear right node
      head = head->right;
      head->prev->right = NULL;
    } else {
      SymbolNode simplify = _simplify(head);
      if (head->prev->left == NULL) {
        head->prev->leftKey     = simplify.key;
        head->prev->leftSymbol  = simplify.symbol;
      } else {
        head->prev->rightKey    = simplify.key;
        head->prev->rightSymbol = simplify.symbol;
      }

      // Traverse up the tree
      ParseNode* prev = head->prev;
      free(head);
      head = prev;
    }
  }

  // Create the first node
  OperationNode* start = malloc(sizeof(*start));

  // Tree fully simplified
  if (_can_simplify(head)) {
    *start = (OperationNode) {
      .value    = _simplify(head),
      .operator = NOP,
      .isLast   = true,
      .endValue = (SymbolNode) {
        .key    = NONE,
        .symbol = NOP,
      },
    };
  } else {
    // Set initial values
    *start = (OperationNode) {
      .value    = (SymbolNode) {
        .key    = head->leftKey,
        .symbol = head->leftSymbol,
      },
      .operator = head->operator,
      .isLast   = true,
      .endValue = (SymbolNode) {
        .key    = head->rightKey,
        .symbol = head->rightSymbol,
      },
    };
  }

  if (head->prev == NULL) {
    free(head);
    return (Evaluation) {
      .isError    = false,
      .operations = start,
    };
  }

  // Remove the read node
  ParseNode* prev = head->prev;
  free(head);
  head = prev;

  // Traverse the tree
  OperationNode* node = start;
  while (head->prev != NULL || _can_traverse(head)) {
    if (_can_traverse_left(head)) {
      // Traverse to and clear left node
      head = head->left;
      head->prev->left = NULL;
    } else if (_can_traverse_right(head)) {
      // Traverse to and clear right node
      head = head->right;
      head->prev->right = NULL;
    } else if (_can_simplify(head) && head->prev != NULL) {
      SymbolNode simplify = _simplify(head);
      if (head->prev->left == NULL) {
        head->prev->leftKey     = simplify.key;
        head->prev->leftSymbol  = simplify.symbol;
      } else {
        head->prev->rightKey    = simplify.key;
        head->prev->rightSymbol = simplify.symbol;
      }

      // Traverse up the tree
      ParseNode* prev = head->prev;
      free(head);
      head = prev;
    } else {
      // Grab whichever valuea have not been evaluated
      if (head->leftKey != NONE) {
        // Update previous node
        SymbolNode lastVal = node->endValue;

        node->next    = malloc(sizeof(*node));
        node->isLast  = false;

        SymbolNode newVal = (SymbolNode) {
          .key    = head->leftKey,
          .symbol = head->leftSymbol,
        };

        // Create the new node
        node = node->next;
        *node = (OperationNode) {
          .value    = lastVal,
          .operator = head->operator,
          .isLast   = true,
          .endValue = newVal,
        };
      }
      if (head->rightKey != NONE) {
        // Update previous node
        SymbolNode lastVal = node->endValue;

        node->next    = malloc(sizeof(*node));
        node->isLast  = false;

        SymbolNode newVal = (SymbolNode) {
          .key    = head->rightKey,
          .symbol = head->rightSymbol,
        };

        // Create the new node
        node = node->next;
        *node = (OperationNode) {
          .value    = lastVal,
          .operator = head->operator,
          .isLast   = true,
          .endValue = newVal,
        };
      }

      // Traverse up the tree
      ParseNode* prev = head->prev;
      free(head);
      head = prev;
    }
  }

  // Grab whichever valuea have not been evaluated
  if (head->leftKey != NONE) {
    // Update previous node
    SymbolNode lastVal = node->endValue;

    node->next    = malloc(sizeof(*node));
    node->isLast  = false;

    SymbolNode newVal = (SymbolNode) {
      .key    = head->leftKey,
      .symbol = head->leftSymbol,
    };

    // Create the new node
    node = node->next;
    *node = (OperationNode) {
      .value    = lastVal,
      .operator = head->operator,
      .isLast   = true,
      .endValue = newVal,
    };
  }
  if (head->rightKey != NONE) {
    // Update previous node
    SymbolNode lastVal = node->endValue;

    node->next    = malloc(sizeof(*node));
    node->isLast  = false;

    SymbolNode newVal = (SymbolNode) {
      .key    = head->rightKey,
      .symbol = head->rightSymbol,
    };

    // Create the new node
    node = node->next;
    *node = (OperationNode) {
      .value    = lastVal,
      .operator = head->operator,
      .isLast   = true,
      .endValue = newVal,
    };
  }

  free_expression(head);

  // Return the final evaluation
  return (Evaluation) {
    .isError    = false,
    .operations = start,
  };
}
