#include "build_ast.h"
#include "error_report.h"
#include "errors.h"
#include "parser.h"

#include <float.h>
#include <stdbool.h>
#include <stdlib.h>

#include <stdio.h>

/* Functions for determing character type */
static bool
_is_num(char ch, int base) {
  switch (base) {
    case 2: {
      return (ch >= '0') && (ch <= '1');
    }
    case 8: {
      return (ch >= '0') && (ch <= '7');
    }
    case 10: {
      return (ch >= '0') && (ch <= '9');
    }
    case 16: {
      return ((ch >= '0') && (ch <= '9')) || ((ch >= 'a') && (ch <= 'f'));
    }
    default: {
      break;
    }
  }

  return false;
}

static bool
_is_space(char ch) {
  // Read for any valid spaces
  switch (ch) {
    case ' ': case '\n': case '\t': {
      return true;
    }
  }

  // Character is not a space
  return false;
}

/* Functions to help parsing numbers */
static int
_find_base(const char** pos, const char** cur) {
  // Check leading number is not zero
  if (**cur != '0') {
    return 10;
  }

  switch (*(*cur+1)) {
    case 'b': {
      // Binary
      if (_is_num(*(*cur+2), 2)) {
        *cur += 2;
        return 2;
      }
      break;
    }
    case 'x': {
      // Hexidecimal
      if (_is_num(*(*cur+2), 16)) {
        *cur += 2;
        return 16;
      }
      break;
    }
    default: {
      // Octal
      if (_is_num(*(*cur+1), 8)) {
        *cur += 1;
        return 8;
      }

      // Initial value was zero
      return 10;
    }
  }

  // Base is undefined
  return 0;
}

static int
_char_to_num(char ch) {
  return (ch <= '9') ? (ch-'0') : (ch-'a'+10);
}

static bool
_parse_int(const char** cur, Value* value, int base) {
  // Read in integer as its negative version
  value->asInt64 = 0;
  for (; _is_num(**cur, base) && (value->asInt64 > INT64_MIN/base); *cur += 1) {
    value->asInt64 = base*value->asInt64-_char_to_num(**cur);
  }

  // Check for overflow for 64 bit integer
  if (_is_num(**cur, base) && (base*(value->asInt64)-_char_to_num(**cur)) < 0) {
    value->asInt64 = base*value->asInt64-_char_to_num(**cur);
    *cur += 1;
    return true;
  }

  return !_is_num(**cur, base);
}

static bool
_parse_double(const char** cur, Value* value, int base) {
  // Read in double as a negative number
  value->asDouble = value->asInt64;
  for (; _is_num(**cur, base) && (value->asDouble > DBL_MIN/base); *cur += 1) {
    value->asDouble = base*value->asDouble-_char_to_num(**cur);
  }

  // Check for overflow of double
  if (_is_num(**cur, base)) {
    if (_is_num(*(*cur+1), base) || (value->asDouble != DBL_MIN/base)) {
      // Traverse to end of number
      while (_is_num(**cur, base)) {
        *cur += 1;
      }
      return false;
    }

    value->asDouble = base*value->asDouble-_char_to_num(**cur);
    *cur += 1;
  }

  return true;
}

static DataType
_set_integer(const char** pos, const char* cur, Value* value, bool isNeg) {
  // Number overflows if positive
  if (!isNeg && (value->asInt64 == INT64_MIN)) {
    return NONE;
  }

  // Save number in large buffer until simplification
  *pos = cur;
  value->asInt64 = isNeg ? value->asInt64 : -value->asInt64;

  return INTGR64;
}

static DataType
_set_float(const char** pos, const char* cur, Value* value, bool isNeg) {
  // Save number in large buffer until simplification
  *pos = cur;
  value->asDouble = isNeg ? value->asDouble : -value->asDouble;

  return DBL;
}

double
_parse_decimal(const char** cur) {
  // Track value of decimal
  double decimal = 0;
  for (double mag = 10; _is_num(**cur, 10) &&
  mag < DBL_MAX/10; mag *= 10, *cur += 1) {
    decimal += _char_to_num(**cur)/mag;
  }

  return decimal;
}

/* Functions for parsing data types */
static bool
_parse_mutation(const char** pos, Mutation* mutation, ErrorReport* report) {
  // Read in mutations
  switch (**pos) {
    case '-': {
      if (IS_NEG(*mutation) || (*mutation != NRMAL)) {
        *report = (
          IS_NEG(*mutation) ?
          ERR_DBL_NEG(*pos, *pos+1) : ERR_NEG_ORDER(*pos, *pos+1)
        );
        return false;
      }
      SET_NEG(*mutation);
      break;
    }
    case '!': {
      if (IS_BOOL(*mutation)) {
        *report = ERR_TRPL_NOT(*pos, *pos+1);
        return false;
      }
      if (IS_NOT(*mutation)) {
        SET_BOOL(*mutation);
      }
      SET_NOT(*mutation);
      break;
    }
    case '~': {
      if (IS_INV(*mutation)) {
        *report = ERR_DBL_INV(*pos, *pos+1);
        return false;
      }
      SET_INV(*mutation);
      break;
    }

    // Character is not an operator
    default: {
      return false;
    }
  }

  // Move past operator
  *pos += 1;

  return true;
}

static DataType
_parse_num(const char** pos, ASTBranch* branch, ErrorReport* report) {
  const char* cur = *pos;

  // Validate input as number
  if (!_is_num(*cur, 10)) {
    return NONE;
  }

  // Track the sign of the variable
  bool isNeg = IS_NEG(branch->mutation);
  CLEAR_NEG(branch->mutation);

  // Determine the base of the number
  int base = _find_base(pos, &cur);
  if (base == 0) {
    *report = ERR_NUM_PREFIX(cur, cur+2);
    return NONE;
  }

  // Parse assuming number is an integer
  Value* value = &branch->value;
  if (_parse_int(&cur, value, base)) {
    if (*cur != '.') {
      // Return integer type
      DataType type = _set_integer(pos, cur, value, isNeg);
      if (type == NONE) {
        *report = ERR_INT_OVERFLOW(*pos, cur);
      }
      return type;
    }

    // Update the data type for double
    value->asDouble = value->asInt64;
  } else if (!_parse_double(&cur, value, base) || (*cur != '.')) {
    // Check valid float properties
    *report = (
      (*cur != '.') ?
      ERR_INT_OVERFLOW(*pos, cur) : ERR_FLT_OVERFLOW(*pos, cur)
    );
    return NONE;
  }

  // Check valid float representation
  if ((base != 10) || IS_INV(branch->mutation)) {
    *report = (
      (base != 10) ?
      ERR_FLT_BASE(cur, cur+1) : ERR_BIT_FLOAT(*pos-1, *pos)
    );
    return NONE;
  }

  // Track decimal value
  cur += 1;
  value->asDouble -= _parse_decimal(&cur);

  // Return float type
  return _set_float(pos, cur, value, isNeg);
}

static bool
_parse_variable(const char** pos, ASTBranch* branch, ErrorReport* report) {
  return false;
}

static bool
_parse_open_brace(const char** pos) {
  if (**pos != '(') {
    return false;
  }

  *pos += 1;
  return true;
}

static DataType
_parse_type(const char** pos, ASTBranch* branch, ErrorReport* report) {
  // Parse numbers
  DataType type = _parse_num(pos, branch, report);
  switch (type) {
    case DBL: case FLT: case INTGR64: case INTGR32: {
      return type;
    }
    default: {
      break;
    }
  }

  // Parse open braces
  if (_parse_open_brace(pos)) {
    return OPEN_BRACE;
  }

  // Parse variables
  if (_parse_variable(pos, branch, report)) {
    return VAR;
  }

  // No valid type found
  return NONE;
}

DataType
parse_value(const char** pos, ASTBranch* branch, ErrorReport* report) {
  branch->mutation = NRMAL;

  // Save location of operator for errors
  const char* oprtr = *pos;

  // Eat spaces before expression
  while (_is_space(**pos)) {
    *pos += 1;
    oprtr = (*(*pos-1) == '\n') ? *pos : oprtr;
  }

  // Parse leading mutations
  while (_parse_mutation(pos, &branch->mutation, report));
  if (report->isError) {
    return NONE;
  }

  // Parse value with mutations
  DataType type = _parse_type(pos, branch, report);
  if (report->isError) {
    return NONE;
  }

  // Return appropriate error
  if (type == NONE) {
    *report = (
      (branch->mutation != NRMAL) ?
      ERR_ILL_VAL_CHAR(*pos-1, *pos) : ERR_NO_VAL(oprtr, *pos+1)
    );
  }
  return type;
}

const char*
parse_operator(const char** pos, Operator* operator, ErrorReport* report) {
  // Eat white spaces
  while (_is_space(**pos)) {
    *pos += 1;
  }

  switch (**pos) {
    case '+': {
      *operator = ADD;
      break;
    }
    case '-': {
      *operator = SUB;
      break;
    }
    case '*': {
      *operator = MLT;
      break;
    }
    case '/': {
      *operator = DIV;
      break;
    }
    case '&': {
      *operator = AND;
      break;
    }
    case '|': {
      *operator = OR;
      break;
    }
    case '^': {
      *operator = XOR;
      break;
    }

    // Look for close of expression
    case ')': {
      *operator = CLOSE_BRACE;
      *pos      += 1;

      return NULL;
    }

    // Validate end of line
    case ';': {
      *operator = SEMICOLON;
      *pos      += 1;

      return NULL;
    }

    // Character is not an operator
    default: {
      *operator = NOP;
      *report   = ERR_NOP(*pos, *pos+1);
      return NULL;
    }
  }

  // Move past operator
  *pos += 1;

  return (*pos-1);
}
