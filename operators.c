#include "operators.h"

SymbolNode mlt_fn(ParseNode* node) {
  switch (node->leftKey) {
    case FLT:
      switch (node->rightKey) {
        case FLT:
          return (SymbolNode) {
            .key    = FLT,
            .symbol = (Symbol) {
              .floating = node->leftSymbol.floating*node->rightSymbol.floating,
            },
          };
        case INT:
          return (SymbolNode) {
            .key    = FLT,
            .symbol = (Symbol) {
              .floating = node->leftSymbol.floating*node->rightSymbol.integer,
            },
          };
        default:
          break;
      }
    case INT:
      switch (node->rightKey) {
        case FLT:
          return (SymbolNode) {
            .key    = FLT,
            .symbol = (Symbol) {
              .floating = node->leftSymbol.integer*node->rightSymbol.floating,
            },
          };
        case INT:
          return (SymbolNode) {
            .key    = INT,
            .symbol = (Symbol) {
              .integer = node->leftSymbol.integer*node->rightSymbol.integer,
            },
          };
        default:
          break;
      }
    default:
      break;
  }
  return (SymbolNode) {
    .key    = NONE,
    .symbol = (Symbol) {
      .operator = NOP,
    },
  };
}
SymbolNode div_fn(ParseNode* node) {
  // Catch divide by zero error
  switch (node->rightKey) {
    case FLT:
      if (node->rightSymbol.floating == 0) {
        return (SymbolNode) {
          .key    = NONE,
          .symbol = (Symbol) {
            .operator = NOP,
          },
        };
      }
    case INT:
      if (node->rightSymbol.integer == 0) {
        return (SymbolNode) {
          .key    = NONE,
          .symbol = (Symbol) {
            .operator = NOP,
          },
        };
      }
    default:
      break;
  }

  switch (node->leftKey) {
    case FLT:
      switch (node->rightKey) {
        case FLT:
          return (SymbolNode) {
            .key    = FLT,
            .symbol = (Symbol) {
              .floating = node->leftSymbol.floating/node->rightSymbol.floating,
            },
          };
        case INT:
          return (SymbolNode) {
            .key    = FLT,
            .symbol = (Symbol) {
              .floating = node->leftSymbol.floating/node->rightSymbol.integer,
            },
          };
        default:
          break;
      }
    case INT:
      switch (node->rightKey) {
        case FLT:
          return (SymbolNode) {
            .key    = FLT,
            .symbol = (Symbol) {
              .floating = node->leftSymbol.integer/node->rightSymbol.floating,
            },
          };
        case INT:
          return (SymbolNode) {
            .key    = INT,
            .symbol = (Symbol) {
              .integer = node->leftSymbol.integer/node->rightSymbol.integer,
            },
          };
        default:
          break;
      }
    default:
      break;
  }
  return (SymbolNode) {
    .key    = NONE,
    .symbol = (Symbol) {
      .operator = NOP,
    },
  };
}
SymbolNode add_fn(ParseNode* node) {
  switch (node->leftKey) {
    case FLT:
      switch (node->rightKey) {
        case FLT:
          return (SymbolNode) {
            .key    = FLT,
            .symbol = (Symbol) {
              .floating = node->leftSymbol.floating+node->rightSymbol.floating,
            },
          };
        case INT:
          return (SymbolNode) {
            .key    = FLT,
            .symbol = (Symbol) {
              .floating = node->leftSymbol.floating+node->rightSymbol.integer,
            },
          };
        default:
          break;
      }
    case INT:
      switch (node->rightKey) {
        case FLT:
          return (SymbolNode) {
            .key    = FLT,
            .symbol = (Symbol) {
              .floating = node->leftSymbol.integer+node->rightSymbol.floating,
            },
          };
        case INT:
          return (SymbolNode) {
            .key    = INT,
            .symbol = (Symbol) {
              .integer = node->leftSymbol.integer+node->rightSymbol.integer,
            },
          };
        default:
          break;
      }
    default:
      break;
  }
  return (SymbolNode) {
    .key    = NONE,
    .symbol = (Symbol) {
      .operator = NOP,
    },
  };
}
SymbolNode and_fn(ParseNode* node) {
  return (SymbolNode) {
    .key    = INT,
    .symbol = (Symbol) {
      .integer = node->leftSymbol.integer&node->rightSymbol.integer,
    },
  };
}
SymbolNode or_fn(ParseNode* node) {
  return (SymbolNode) {
    .key    = INT,
    .symbol = (Symbol) {
      .integer = node->leftSymbol.integer|node->rightSymbol.integer,
    },
  };
}
SymbolNode xor_fn(ParseNode* node) {
  return (SymbolNode) {
    .key    = INT,
    .symbol = (Symbol) {
      .integer = node->leftSymbol.integer^node->rightSymbol.integer,
    },
  };
}
