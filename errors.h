#define NO_ERR (Error) {  \
  .isError = false,       \
}

#define ERR_NO_EXPRESSION(s, e) (Error) {       \
  .isError = true,                              \
  .message = "no operable expression to parse", \
  .startIdx = (e)-(s),                          \
  .endIdx = (e)-(s)+1,                          \
}

#define ERR_ILLEGAL_START(s, e) (Error) {               \
  .isError = true,                                      \
  .message = "illegal operator at start of expression", \
  .startIdx = (e)-(s)-1,                                \
  .endIdx = (e)-(s),                                    \
}

#define ERR_ILLEGAL_CLOSE_BRACE(s, e) (Error) {               \
  .isError = true,                                            \
  .message = "illegal close brace with no opening argument",  \
  .startIdx = (e)-(s)-1,                                      \
  .endIdx = (e)-(s),                                          \
}

#define ERR_ILLEGAL_OPEN_BRACE(s, e) (Error) {              \
  .isError = true,                                          \
  .message = "illegal open brace with no closing argument", \
  .startIdx = (e)-(s)-1,                                    \
  .endIdx = (e)-(s),                                        \
}

#define ERR_DOUBLE_SYMBOL(s, e) (Error) {                   \
  .isError = true,                                          \
  .message = "double argument with no evaluable operator",  \
  .startIdx = (e)-(s)-1,                                    \
  .endIdx = (e)-(s),                                        \
}

#define ERR_DOUBLE_OPERATOR(s, e) (Error) {                 \
  .isError = true,                                          \
  .message = "double operator with no evaluable argument",  \
  .startIdx = (e)-(s)-1,                                    \
  .endIdx = (e)-(s),                                        \
}

#define ERR_ILLEGAL_ARGUMENT(s, e) (Error) {        \
  .isError = true,                                  \
  .message = "symbol is not an evaluable argument", \
  .startIdx = (e)-(s)-1,                            \
  .endIdx = (e)-(s),                                \
}

#define ERR_END_SYMBOL(s, e) (Error) {                \
  .isError = true,                                    \
  .message = "symbol at end of evaluable expression", \
  .startIdx = (e)-(s)-1,                              \
  .endIdx = (e)-(s),                                  \
}
