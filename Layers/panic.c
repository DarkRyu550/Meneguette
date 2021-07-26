#include "panic.h"
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

void panic(const char* fmt, ...)
{
	va_list va;
	va_start(va, fmt);

	fprintf(stderr, "The program has panicked!\nMessage: ");
	vfprintf(stderr, fmt, va);
	fprintf(stderr, "\n");
    fflush(stderr);

	va_end(va);
	abort();
}
