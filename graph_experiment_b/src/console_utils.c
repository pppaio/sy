#include <stdio.h>
#include <string.h>
#include "../include/console_utils.h"

#ifdef _WIN32
#include <windows.h>
#include <stdlib.h>
#endif

void consoleInitEncoding(void)
{
#if defined(_WIN32) && defined(CONSOLE_FORCE_UTF8)
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
    system("chcp 65001 >nul 2>nul");
#endif
}

void consoleClearInputBuffer(void)
{
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) { }
}

int consoleReadInt(int defaultValue)
{
    int result;
    if (scanf("%d", &result) != 1) {
        consoleClearInputBuffer();
        return defaultValue;
    }
    return result;
}

void consoleReadLine(char *buffer, size_t bufferSize)
{
    if (buffer == NULL || bufferSize == 0) {
        return;
    }
    if (fgets(buffer, (int)bufferSize, stdin) == NULL) {
        buffer[0] = '\0';
        return;
    }
    consoleTrimNewLine(buffer);
}

void consoleTrimNewLine(char *buffer)
{
    size_t length;
    if (buffer == NULL) {
        return;
    }
    length = strlen(buffer);
    if (length > 0 && buffer[length - 1] == '\n') {
        buffer[length - 1] = '\0';
    }
}

void consolePrintDivider(char symbol, int length)
{
    int i;
    for (i = 0; i < length; i++) {
        putchar(symbol);
    }
    putchar('\n');
}

void consolePrintVertexSummary(const GraphVertex *vertex)
{
    if (vertex == NULL) {
        return;
    }
    printf("V%d:%s", vertex->vertexId, vertex->vertexName);
    if (strlen(vertex->vertexDesc) > 0) {
        printf(" (%s)", vertex->vertexDesc);
    }
}
