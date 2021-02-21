#ifndef _LINUX_STRING_H_
#define _LINUX_STRING_H_

#include <linux/types.h>	/* for size_t */

#ifndef NULL
#define NULL ((void *) 0)
#endif

/*
 * This string-include defines all string functions as inline
 * functions. Use gcc. It also assumes ds=es=data space, this should be
 * normal. Most of the string-functions are rather heavily hand-optimized,
 * see especially strtok,strstr,str[c]spn. They should work, but are not
 * very easy to understand. Everything is done entirely within the register
 * set, making the functions fast and clean. String instructions have been
 * used through-out, making for "slightly" unclear code :-)
 *
 *		Copyright (C) 1991, 1992 Linus Torvalds
 */
 
extern inline char * strcpy(char * dest,const char *src);

extern inline char * strncpy(char * dest,const char *src,size_t count);

extern inline char * strcat(char * dest,const char * src);

extern inline char * strncat(char * dest,const char * src,size_t count);

extern inline int strcmp(const char * cs,const char * ct);

extern inline int strncmp(const char * cs,const char * ct,size_t count);

extern inline char * strchr(const char * s,char c);

extern inline char * strrchr(const char * s,char c);

extern inline size_t strspn(const char * cs, const char * ct);
extern inline size_t strcspn(const char * cs, const char * ct);

extern inline char * strpbrk(const char * cs,const char * ct);

extern inline char * strstr(const char * cs,const char * ct);

extern inline size_t strlen(const char * s);

extern char * ___strtok;

extern inline char * strtok(char * s,const char * ct);

extern inline void * memcpy(void * to, const void * from, size_t n);

extern inline void * memmove(void * dest,const void * src, size_t n);

extern inline int memcmp(const void * cs,const void * ct,size_t count);

extern inline void * memchr(const void * cs,char c,size_t count);

extern inline void * memset(void * s,char c,size_t count);

#endif
