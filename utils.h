#ifndef UTILS_H
#define UTILS_H
#include <time.h>
#include <stdbool.h>
#include <windows.h>

void closeProgram(void);
void loadConsole(void);
void setHooks(void);
int random(int from, int to);
time_t millis(void);
bool cmpTcharStr(const TCHAR *s1, const TCHAR *s2);
#endif
