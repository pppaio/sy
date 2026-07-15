#ifndef CONSOLE_UTILS_H
#define CONSOLE_UTILS_H

#include "graph_interface.h"

void consoleInitEncoding(void);
void consoleClearInputBuffer(void);
int consoleReadInt(int defaultValue);
void consoleReadLine(char *buffer, size_t bufferSize);
void consoleTrimNewLine(char *buffer);
void consolePrintDivider(char symbol, int length);
void consolePrintVertexSummary(const GraphVertex *vertex);

#endif
