#ifndef __AST_STRUCTS_H__
#define __AST_STRUCTS_H__

#include <stdbool.h>
#include <stdint.h>

// Parsable types
typedef struct _Value {
  union {
    int32_t asInt32;
    float   asFloat;
    double  asDouble;
    int64_t asInt64;
    char*   asVariable;
  };
} Value;

// Track each possible data type
typedef enum {
  NONE, BRNCH, OPEN_BRACE, DBL, FLT, INTGR64, INTGR32, VAR,
} DataType;

// Operators acting on one value
typedef enum {
  NRMAL, NEG=(1<<0), NOT=(1<<1), INV=(1<<2), BOOL=(1<<3),
  // --, -n        , !n        , ~n        , !!n        ,
} Mutation;

// Operators acting on two values
typedef enum {
  NOP, CLOSE_BRACE, MLT, DIV, ADD, SUB, AND, OR, XOR, SEMICOLON
} Operator;

// Disambiguate the type for assignemnt
typedef struct _ASTNode* Data;

// Data for each abstract syntax tree branch
typedef struct {
  // Track data of the node
  DataType dataType;
  Mutation mutation;

  // Store each possible type
  union {
    Data              data;

    struct _ASTNode*  branch;
    Value             value;
  };
} ASTBranch;

// Node for abstract syntax tree
typedef struct _ASTNode {
  // Values for internal checking
  union {
    const char* oprtrPos;
    bool*       isNotVisited;
  };

  // Head information of each node
  Operator          oprtr;
  struct _ASTNode*  prev;

  // Branches of the abstract syntax tree
  ASTBranch left;
  ASTBranch right;
} ASTNode;

// Head of abstract syn tree
typedef struct {
  Mutation mutation;
  ASTNode* head;
} AST;

#endif
