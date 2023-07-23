#include "error_report.h"

#include <stdio.h>

/* Functions for finding location of error */
static uint32_t
_find_line_num(const char* str, uint32_t startIdx) {
  // Count number of lines before error
  uint32_t line = 1;
  for (uint32_t i = 0; i < startIdx; i += 1) {
    line += (str[i] == '\n');
  }

  return line;
}

static uint32_t
_find_line_start(const char* str, uint32_t startIdx) {
  // Count characters until start of line
  uint32_t lineStart = startIdx;
  while (str[lineStart] != '\n') {
    lineStart -= 1;
  }

  return lineStart+1;
}

static uint32_t
_find_line_end(const char* str, uint32_t endIdx) {
  // Count character until end of line
  uint32_t lineEnd = endIdx;
  while (str[lineEnd] != '\n' && str[lineEnd] != '\0') {
    lineEnd += 1;
  }

  return lineEnd;
}

/* Functions for displaying error */
static void
_print_error_message(const char* str, ErrorReport report,
uint32_t line, uint32_t lineStart, uint32_t lineEnd) {
  uint32_t startIdx = report.startAddr-str;
  uint32_t endIdx   = report.endAddr-str;
  
  // Display error message
  fprintf(stderr, BOLD_RESET "line %u: " BOLD_RED "error: "
    BOLD_RESET "%s\n" TEXT_RESET, line, report.message);

  // Display error line
  for (uint32_t i = lineStart; i < lineEnd-1; i += 1) {
    fputc(str[i], stderr);
  }
  fprintf(stderr, "%c" BOLD_GREEN "\n",
    (str[lineEnd-1] == '\n') ? ' ' : str[lineEnd-1]);

  // Emphasize error characters
  for (uint32_t i = lineStart; i < startIdx; i += 1) {
    fputc(' ', stderr);
  }
  for (uint32_t i = startIdx; i < endIdx; i += 1) {
    fputc('^', stderr);
  }
  fprintf(stderr, TEXT_RESET "\n");

  return;
}

bool
is_error(const char* str, ErrorReport report) {
  // Do nothing if no error
  if (!report.isError) {
    return false;
  }

  uint32_t startIdx = report.startAddr-str;
  uint32_t endIdx   = report.endAddr-str;

  // Track line of error
  uint32_t line = _find_line_num(str, startIdx);

  // Find bounds of error line
  uint32_t lineStart  = (line > 1) ? _find_line_start(str, startIdx) : 0;
  uint32_t lineEnd    = _find_line_end(str, endIdx);

  // Print error report
  _print_error_message(str, report, line, lineStart, lineEnd);

  return true;
}
