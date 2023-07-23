#ifndef __PARSER_H__
#define __PARSER_H__

#include "ast_structs.h"
#include "error_report.h"

// Set mutation signs
#define SET_NEG(m)  (m |= NEG)
#define SET_NOT(m)  (m |= NOT)
#define SET_INV(m)  (m |= INV)
#define SET_BOOL(m) (m |= BOOL)

// Clear mutation for negative values
#define CLEAR_MUTS(m) (m = NRMAL)
#define CLEAR_NEG(m)  (m &= ~NEG)

// Read mutation signs
#define IS_NEG(m)   (((m)/NEG)&0x01)
#define IS_NOT(m)   (((m)/NOT)&0x01)
#define IS_INV(m)   (((m)/INV)&0x01)
#define IS_BOOL(m)  (((m)/BOOL)&0x01)

// Parse value expressions
DataType
parse_value(const char** pos, ASTBranch* node, ErrorReport* report);

// Parse operators
const char*
parse_operator(const char** pos, Operator* operator, ErrorReport* report);

#endif
