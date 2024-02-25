#include <gff/debug.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

extern int gff_debug(const char *file, const int line_num, const char *pretty, const char *str, ...) {
    //if (!debug) { return EXIT_SUCCESS; }
    va_list argp;
    va_start(argp, str);
    printf("[%s:%d] %s: ", file, line_num, pretty);
    vprintf(str, argp);
    return EXIT_SUCCESS;
}
