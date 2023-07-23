#include "build_ast.h"
#include "errors.h"
#include "parser.h"

#include <stdlib.h>

/* Functions for freeing abstract syntax tree */
static void
_free_unmalloced_node_tree(ASTNode* node) {
  // Set empty abstract syntax tree
  AST ast = {
    .mutation = NRMAL,
    .head     = NULL,
  };

  // Remove connection to left node
  if (node->left.dataType == BRNCH) {
    ast.head        = node->left.branch,
    ast.head->prev  = NULL;
    free_ast(&ast);
  }

  // Remove connection to right node
  if (node->right.dataType == BRNCH) {
    ast.head        = node->right.branch,
    ast.head->prev  = NULL;
    free_ast(&ast);
  }

  // Remove connection to previous node
  if (node->prev != NULL) {
    // Remove connection to previous branch
    ASTBranch prevLeft = node->prev->left;
    if ((prevLeft.dataType == BRNCH) && (prevLeft.branch == node)) {
      node->prev->left.dataType   = NONE;
    } else {
      node->prev->right.dataType  = NONE;
    }

    ast.head = node,
    free_ast(&ast);
  }

  return;
}

void
free_ast(AST* ast) {
  // Avoid freeing empty tree
  if (ast == NULL) {
    return;
  }

  // Free tree with one node
  if (ast->head->oprtr == NOP) {
    free(ast->head);
    *ast = (AST) {
      .mutation = NRMAL,
      .head     = NULL,
    };
    return;
  }

  // Traverse to head of tree
  while (ast->head->prev != NULL) {
    ast->head = ast->head->prev;
  }

  // Free unsimplified tree
  for (ASTNode* temp = ast->head; temp != NULL; temp = ast->head) {
    // Traverse to bottom of nodes
    while (temp->left.dataType == BRNCH || temp->right.dataType == BRNCH) {
      if (temp->left.dataType == BRNCH) {
        temp->left.dataType   = NONE;
        temp                  = temp->left.branch;
      } else {
        temp->right.dataType  = NONE;
        temp                  = temp->right.branch;
      }
    }

    // Remove node
    ast->head = temp->prev;
    free(temp);
  }

  // Set abstract syntax tree as empty
  *ast = (AST) {
    .mutation = NRMAL,
    .head     = NULL,
  };

  return;
}

/* Functions for connecting nodes to their correct neightbors */
static void
_connect_children_nodes(ASTNode* node) {
  // Attach children node to parent
  if (node->left.dataType == BRNCH) {
    node->left.branch->prev = node;
  }
  if (node->right.dataType == BRNCH) {
    node->right.branch->prev = node;
  }

  return;
}

static void
_swap_head_and_node(ASTNode* head, ASTNode* node) {
  // Do nothing if head already set
  if (head == node) {
    return;
  }

  // Swap values of nodes
  ASTNode temp  = *head;
  *head         = *node;
  *node         = temp;

  // Avoid node connecting to self
  if ((head->left.dataType == BRNCH) && (head->left.branch == head)) {
    head->left.branch   = node;
    node->prev          = head;
  } else if ((head->right.dataType == BRNCH) && (head->right.branch == head)) {
    head->right.branch  = node;
    node->prev          = head;
  } else {

    // Link parent to new child
    ASTBranch prevLeft = node->prev->left;
    if ((prevLeft.dataType == BRNCH) && (prevLeft.branch == head)) {
      node->prev->left.branch   = node;
    } else {
      node->prev->right.branch  = node;
    }
  }

  // Connect children to appropriate nodes
  _connect_children_nodes(head);
  _connect_children_nodes(node);

  return;
}

/* Functions for parsing subtrees in abstract syntax tree */
static Operator
_read_line(const char** pos, ASTNode* head, ErrorReport* report);

static void
_read_braces(const char** pos, ASTNode* prev,
ASTBranch* branch, ErrorReport* report) {
  // Avoid parse if no subtree needed
  if (branch->dataType != OPEN_BRACE) {
    return;
  }

  // Save position of opening brace
  const char* openBrace = *pos-1;

  // Set empty node for subtree
  ASTNode head = EMPTY_AST_NODE;

  // Read arguments in braces
  Operator oprtr = _read_line(pos, &head, report);
  if (oprtr != CLOSE_BRACE) {
    if (!report->isError) {
      *report = ERR_NO_CLOSE_BRACE(openBrace, openBrace+1);
    }
    _free_unmalloced_node_tree(&head);
    return;
  }

  // Check for single value in parenthesis
  if (head.right.dataType == NONE) {
    *branch = head.left;
  } else {
    // Subtree built in parenthesis
    branch->branch  = malloc(sizeof(*branch->branch));
    *branch->branch = head;

    _connect_children_nodes(branch->branch);

    branch->dataType      = BRNCH;
    branch->branch->prev  = prev;
  }

  return;
}

/* Functions for inserting nodes into correct location */
static bool
_node_less_priority(ASTNode* node, Operator oprtr) {
  // Make operations with the same priority equal
  Operator nodeOprtr = node->oprtr;
  switch (nodeOprtr) {
    case DIV: case SUB: case XOR: {
      nodeOprtr -= 1;
      break;
    }
    default: {
      break;
    }
  }
  switch (oprtr) {
    case DIV: case SUB: case XOR: {
      oprtr -= 1;
      break;
    }
    default: {
      break;
    }
  }

  return nodeOprtr <= oprtr;
}

static ASTNode*
_insert_node(ASTNode* node, Operator oprtr, const char* oprtrPos) {
  // Traverse to correct priority node
  while (node->prev != NULL && _node_less_priority(node, oprtr)) {
    node = node->prev;
  }

  // Create new head
  if (_node_less_priority(node, oprtr)) {
    ASTNode* head = malloc(sizeof(*head));
    *head         = (ASTNode) {
      .oprtr    = oprtr,
      .oprtrPos = oprtrPos,
      .prev     = NULL,

      .left = (ASTBranch) {
        .dataType = BRNCH,
        .mutation = NRMAL,
        .branch   = node,
      },
      .right = (ASTBranch) {
        .dataType = BRNCH,
        .mutation = NRMAL,
        .branch   = NULL,
      },
    };

    node->prev = head;
    return head;
  }

  // Copy data from right
  Data data = node->right.data;

  node->right.branch  = malloc(sizeof(*node));
  *node->right.branch = (ASTNode) {
    .oprtr    = oprtr,
    .oprtrPos = oprtrPos,
    .prev     = node,

    .left = (ASTBranch) {
      .dataType = node->right.dataType,
      .mutation = node->right.mutation,
      .data     = data,
    },
    .right = (ASTBranch) {
      .dataType = BRNCH,
      .mutation = NRMAL,
      .branch   = NULL,
    },
  };

  // Connect subtree to new node
  if (node->right.dataType == BRNCH) {
    data->prev = node->right.branch;
  }
  node->right.dataType = BRNCH;

  return node->right.branch;
}

static bool
_set_node(const char** pos, ASTNode* node,
ASTBranch* branch, ErrorReport* report) {
  branch->dataType = parse_value(pos, branch, report);
  _read_braces(pos, node, branch, report);

  return !report->isError;
}

/* Functions for parsing line into abstract syntax tree */
static Operator
_read_line(const char** pos, ASTNode* head, ErrorReport* report) {
  ASTNode* node = head;

  // Set first node
  if (!_set_node(pos, node, &node->left, report)) {
    return node->oprtr;
  }
  node->oprtrPos = parse_operator(pos, &node->oprtr, report);
  if (node->oprtrPos == NULL) {
    return node->oprtr;
  }
  if (!_set_node(pos, node, &node->right, report)) {
    return node->oprtr;
  }

  // Set rest of nodes
  Operator oprtr;
  for (const char* oprtrPos = parse_operator(pos, &oprtr, report);
  oprtrPos != NULL; oprtrPos = parse_operator(pos, &oprtr, report)) {
    // Insert new node into left value
    node = _insert_node(node, oprtr, oprtrPos);

    // Save new value into right node
    if (!_set_node(pos, node, &node->right, report)) {
      return oprtr;
    }
  }

  // Catch any error from parsing
  if (report->isError) {
    return oprtr;
  }

  // Find head of tree
  while (node->prev != NULL) {
    node = node->prev;
  }

  // Save passed in node to head
  _swap_head_and_node(head, node);

  return oprtr;
}

AST
build_line_ast(const char* str, ErrorReport* report) {
  // Assume no errors
  *report = NO_ERR;

  // Set values for function parsing
  const char* pos = str;
  ASTNode node = EMPTY_AST_NODE;

  // Parse line to build abstract syntax tree
  Operator oprtr = _read_line(&pos, &node, report);
  if (((*pos == '\0') && (node.oprtr != SEMICOLON)) || (oprtr == CLOSE_BRACE)) {
    *report = (
      (oprtr == CLOSE_BRACE) ?
      ERR_NO_OPEN_BRACE(pos-1, pos) : ERR_NO_SEMICOLON(pos-1, pos)
    );
  }

  // Set empty abstract syntax tree
  AST ast = {
    .mutation = NRMAL,
    .head     = NULL,
  };

  // Free data if error occured
  if (report->isError) {
    _free_unmalloced_node_tree(&node);
    return ast;
  }

  // Account for entire expression in parenthesis
  if (node.left.dataType == BRNCH && node.right.dataType == NONE) {
    node.left.branch->prev = NULL;
    ast = (AST) {
      .mutation = node.left.mutation,
      .head     = node.left.branch,
    };
    return ast;
  }

  // Create space for unmalloced node
  ast = (AST) {
    .mutation = NRMAL,
    .head     = malloc(sizeof(*ast.head)),
  };
  *ast.head = node;

  // Connect subtrees to head node
  _connect_children_nodes(ast.head);

  return ast;
}
