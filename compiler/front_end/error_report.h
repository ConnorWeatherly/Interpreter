#ifndef __ERROR_REPORT_H__
#define __ERROR_REPORT_H__

#include <stdbool.h>
#include <stdint.h>

typedef struct {
  // isError must be an address to save space
  union {
    bool*       isError;
    const char* message;
  };

  // Start and end of the found error
  const char* startAddr;
  const char* endAddr;
} ErrorReport;

// Text colors from printing error messages
#define BOLD_GREEN  "\x1b[32;1m"
#define BOLD_RED    "\x1b[31;1m"
#define BOLD_RESET  "\x1b[0;1m"
#define TEXT_RESET  "\x1b[0m"

// Error handling and display
bool
is_error(const char* str, ErrorReport report);

#endif
