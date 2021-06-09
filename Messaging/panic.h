#ifndef __PANIC_H__
#define __PANIC_H__

/** Invoke a panic and abort the program with the given message. */
#if defined(__has_attribute) && __has_attribute(noreturn)
//silence warnings in cases such as
//  if(x == null) panic("x was null");
//  x->y = 1; //without this attribute this would be a warning
__attribute__((noreturn))
#endif
#if defined(__has_attribute) && __has_attribute(format)
//detect wrong format string usage such as
//  if(x != 10) panic("x (%s) was not 10", x);
__attribute__((format (printf, 1, 2)))
#endif
void panic(const char *fmt, ...);

#endif
