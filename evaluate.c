#include <stdlib.h>
#include "evaluate.h"

Evaluation evaluate(ParseNode* head) {
  // Take care of the empty case
  if (head == NULL) {
    return (Evaluation) {
      .size       = 0,
      .operations = NULL,
    };
  }
}
