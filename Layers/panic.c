#include "panic.h"
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
//for backtrace
#include <execinfo.h>
#include <unistd.h>

void panic(const char* fmt, ...)
{
	va_list va;
	va_start(va, fmt);

	fprintf(stderr, "The program has panicked!\nMessage: ");
	vfprintf(stderr, fmt, va);
	fprintf(stderr, "\n");
    fflush(stderr);

    const int buffer_size = 100;
    void* buffer[buffer_size];
    int symbols = backtrace(buffer, buffer_size);
    backtrace_symbols_fd(buffer, symbols, STDERR_FILENO);

	va_end(va);
	abort();
}
