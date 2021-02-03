#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

void error(const char *msg, int code, ...) {
    va_list args;
    va_start(args, code);
    fprintf(stderr, "Error: ");
    vfprintf(stderr, msg, args);
    fprintf(stderr, "\n");
    va_end(args);
    exit(code);
}

void warning(const char *msg, ...) {
    va_list args;
    va_start(args, msg);
    fprintf(stderr, "Warning: ");
    vfprintf(stderr, msg, args);
    fprintf(stderr, "\n");
    va_end(args);
}

void logit(const char *msg, ...) {
    va_list args;
    va_start(args, msg);
    vfprintf(stdout, msg, args);
    fprintf(stdout, "\n");
    va_end(args);
}
