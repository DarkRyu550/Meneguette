#ifndef __PANIC_H__
#define __PANIC_H__

/** Invoke a panic and abort the program with the given message. */
#if defined(__has_attribute) && __has_attribute(noreturn)
__attribute__((noreturn))
#endif
#if defined(__has_attribute) && __has_attribute(format)
__attribute__((format (printf, 1, 2)))
#endif
void panic(const char *fmt, ...);

#endif
