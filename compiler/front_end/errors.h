#ifndef __ERRORS_H__
#define __ERRORS_H__

#define NO_ERR (ErrorReport) {  \
  .isError = NULL,              \
}

#define ERR_NUM_PREFIX(s, e) (ErrorReport) {            \
  .message    = "illegal prefix to unspecified number", \
  .startAddr  = s,                                      \
  .endAddr    = e,                                      \
}

#define ERR_INT_OVERFLOW(s, e) (ErrorReport) {                \
  .message    = "value too large to store in 64 bit integer", \
  .startAddr  = s,                                            \
  .endAddr    = e,                                            \
}

#define ERR_FLT_BASE(s, e) (ErrorReport) {              \
  .message    = "floats must be expressed in base 10",  \
  .startAddr  = s,                                      \
  .endAddr    = e,                                      \
}

#define ERR_FLT_OVERFLOW(s, e) (ErrorReport) {                          \
  .message    = "value too large to store in double percission float",  \
  .startAddr  = s,                                                      \
  .endAddr    = e,                                                      \
}

#define ERR_NOP(s, e) (ErrorReport) {                         \
  .message    = "value without operator cannot be evaluated", \
  .startAddr  = s,                                            \
  .endAddr    = e,                                            \
}

#define ERR_BIT_FLOAT(s, e) (ErrorReport) {             \
  .message    = "floats do not have bitwise opertions", \
  .startAddr  = s,                                      \
  .endAddr    = e,                                      \
}

#define ERR_DBL_NEG(s, e) (ErrorReport) {         \
  .message    = "illegale double negative sign",  \
  .startAddr  = s,                                \
  .endAddr    = e,                                \
}

#define ERR_NEG_ORDER(s, e) (ErrorReport) {             \
  .message    = "negative sign must be first modifier", \
  .startAddr  = s,                                      \
  .endAddr    = e,                                      \
}

#define ERR_DBL_INV(s, e) (ErrorReport) {       \
  .message    = "illegale double inverse sign", \
  .startAddr  = s,                              \
  .endAddr    = e,                              \
}

#define ERR_TRPL_NOT(s, e) (ErrorReport) {  \
  .message    = "illegale triple not sign", \
  .startAddr  = s,                          \
  .endAddr    = e,                          \
}

#define ERR_ILL_VAL_CHAR(s, e) (ErrorReport) {              \
  .message    = "illegale character after value modifier",  \
  .startAddr  = s,                                          \
  .endAddr    = e,                                          \
}

#define ERR_NO_VAL(s, e) (ErrorReport) {                  \
  .message    = "expression must contain operable value", \
  .startAddr  = s,                                        \
  .endAddr    = e,                                        \
}

#define ERR_NO_SEMICOLON(s, e) (ErrorReport) {        \
  .message    = "expression must end with semicolon", \
  .startAddr  = s,                                    \
  .endAddr    = e,                                    \
}

#define ERR_NO_CLOSE_BRACE(s, e) (ErrorReport) {              \
  .message    = "open brace does not have closing argument",  \
  .startAddr  = s,                                            \
  .endAddr    = e,                                            \
}

#define ERR_NO_OPEN_BRACE(s, e) (ErrorReport) {               \
  .message    = "close brace does not have opening argument", \
  .startAddr  = s,                                            \
  .endAddr    = e,                                            \
}

#define ERR_OPP_INT_OVERFLOW(s, e) (ErrorReport) {                    \
  .message    = "value after operation cannot fit in 64 bit integer", \
  .startAddr  = s,                                                    \
  .endAddr    = e,                                                    \
}

#define ERR_OPP_DBL_OVERFLOW(s, e) (ErrorReport) {                  \
  .message    = "value after operation cannot fit in 64 bit float", \
  .startAddr  = s,                                                  \
  .endAddr    = e,                                                  \
}

#define ERR_DIV_BY_ZERO(s, e) (ErrorReport) {                   \
  .message    = "divide by zero results in undefined behavior", \
  .startAddr  = s,                                              \
  .endAddr    = e,                                              \
}

#endif
