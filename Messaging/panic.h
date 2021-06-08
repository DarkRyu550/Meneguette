#ifndef __PANIC_H__
#define __PANIC_H__

/** Invoke a panic and abort the program with the given message. */
void panic(const char *fmt, ...);

#endif
