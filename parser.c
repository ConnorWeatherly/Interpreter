#include <stdlib.h>
#include <float.h>
#include "parser.h"

static bool _is_num(char ch) {
  return ('0' <= ch) && ('9' >= ch);
}
bool is_space(char ch) {
  // Read for any valid spaces
  switch (ch) {
    case ' ': case '\n': case '\t':
    return true;
  }

  // Character is not a space
  return false;
}

static bool _parse_open_parenthesis(char** pos) {
  // Check for open parenthesis
  if (**pos != '(') {
    return false;
  }

  // Move past open parenthesis
  *pos += 1;
  return true;
}
static bool _parse_close_parenthesis(char** pos) {
  // Check for close parenthesis
  if (**pos != ')') {
    return false;
  }

  // Move past close parenthesis
  *pos += 1;
  return true;
}
static bool _parse_float(char** pos, float* num) {
  // Allow for moving through string without changing position
  char* str = *pos;

  // Check if number is negative
  bool isNeg = false;
  if (*str == '-') {
    isNeg = true;
    str += 1;
  }

  // Check if number is numid
  if (!_is_num(*str)) {
    return false;
  }

  // Save negative number into buffer
  *num = 0;
  for (; _is_num(*str); str += 1) {
    *num = 10*(*num)-(*str)+'0';
  }

  // Check the number is a float
  if (*str != '.') {
    return false;
  }
  str += 1;

  // Tack on decimal values to end
  for (float mag = 10; _is_num(*str) && mag < DBL_MAX/10; mag *= 10, str += 1) {
    *num -= (*str-'0')/mag;
  }

  // Flip the negative value to positive
  if (!isNeg) {
    *num = -(*num);
  }

  // Move the position of the string to after the number
  *pos = str;

  return true;
}
static bool _parse_integer(char** pos, int* num) {
  // Allow for moving through string without changing position
  char* str = *pos;

  // Check if number is negative
  bool isNeg = false;
  if (*str == '-') {
    isNeg = true;
    str += 1;
  }

  // Check if number is numid
  if (!_is_num(*str)) {
    return false;
  }

  // Save negative number into buffer
  *num = 0;
  for (; _is_num(*str); str += 1) {
    *num = 10*(*num)-(*str)+'0';
  }

  // Flip the negative value to positive
  if (!isNeg) {
    *num = -(*num);
  }

  // Move the position of the string to after the number
  *pos = str;

  return true;
}
static bool _parse_operation(char** pos, Operation* operator) {
  // Run through all valid operations
  switch (**pos) {
    case '+':
      *operator = ADD;
      break;
    case '-':
      *operator = SUB;
      break;
    case '*':
      *operator = MLT;
      break;
    case '/':
      *operator = DIV;
      break;
    case '&':
      *operator = AND;
      break;
    case '|':
      *operator = OR;
      break;
    case '^':
      *operator = XOR;
      break;
    case '~':
      *operator = NOT;
      break;

    // Character is not an operation
    default:
      return false;
  }

  // Move past operator
  *pos += 1;

  return true;
}
static bool _parse_variable(char** pos, char** variable) {
  return false;
}

SymbolKey parse_symbol(char** pos, Symbol* symbol) {
  // Eat blank spaces
  while (is_space(**pos)) {
    *pos += 1;
  }

  // Check symbol for each type
  if (_parse_open_parenthesis(pos)) {
    return OPN_PAR;
  } else if (_parse_close_parenthesis(pos)) {
    return CLS_PAR;
  } else if (_parse_float(pos, &symbol->floating)) {
    return FLT;
  } else if (_parse_integer(pos, &symbol->integer)) {
    return INT;
  } else if (_parse_operation(pos, &symbol->operator)) {
    return OPRTR;
  } else if (_parse_variable(pos, &symbol->variable)) {
    return VAR;
  }

  // Symbol is not parsable
  return NONE;
}
