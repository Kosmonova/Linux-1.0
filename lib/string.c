/*
 *  linux/lib/string.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 */

#ifndef __GNUC__
#error I want gcc!
#endif

#include <linux/types.h>

#define extern
#define inline
#define __LIBRARY__
#include <linux/string.h>


inline char * strcpy(char * dest,const char *src)
{
__asm__(
	"cld\n"
	"1:\tlodsb\n\t"
	"stosb\n\t"
	"testb %%al,%%al\n\t"
	"jne 1b"
	: /* no output */
	:"S" (src),"D" (dest):"ax", "memory");
return dest;
}

inline char * strncpy(char * dest,const char *src,size_t count)
{
__asm__("cld\n"
	"1:\tdecl %2\n\t"
	"js 2f\n\t"
	"lodsb\n\t"
	"stosb\n\t"
	"testb %%al,%%al\n\t"
	"jne 1b\n\t"
	"rep\n\t"
	"stosb\n\t"
	"2:"
	: /* no output */
	:"S" (src),"D" (dest),"c" (count):"ax","memory");
return dest;
}

inline char * strcat(char * dest,const char * src)
{
__asm__("cld\n\t"
	"repne\n\t"
	"scasb\n\t"
	"decl %1\n"
	"1:\tlodsb\n\t"
	"stosb\n\t"
	"testb %%al,%%al\n\t"
	"jne 1b"
	: /* no output */
	:"S" (src),"D" (dest),"a" (0),"c" (0xffffffff):);
return dest;
}

inline char * strncat(char * dest,const char * src,size_t count)
{
__asm__("cld\n\t"
	"repne\n\t"
	"scasb\n\t"
	"decl %1\n\t"
	"movl %4,%3\n"
	"1:\tdecl %3\n\t"
	"js 2f\n\t"
	"lodsb\n\t"
	"stosb\n\t"
	"testb %%al,%%al\n\t"
	"jne 1b\n"
	"2:\txorl %2,%2\n\t"
	"stosb"
	: /* no output */
	:"S" (src),"D" (dest),"a" (0),"c" (0xffffffff),"g" (count)
	:"memory");
return dest;
}

// inline int strcmp(const char * cs,const char * ct)
// {
// register int __res __asm__("ax");
// __asm__("pushl %%esi\n\t"
// 	"pushl %%edi":);
// __asm__("cld\n"
// 	"1:\tlodsb\n\t"
// 	"scasb\n\t"
// 	"jne 2f\n\t"
// 	"testb %%al,%%al\n\t"
// 	"jne 1b\n\t"
// 	"xorl %%eax,%%eax\n\t"
// 	"jmp 3f\n"
// 	"2:\tmovl $1,%%eax\n\t"
// 	"jb 3f\n\t"
// 	"negl %%eax\n"
// 	"3:"
// 	:"=a" (__res):"D" (cs),"S" (ct));
// __asm__("popl %%edi\n\t"
// 	"popl %%esi":);
// return __res;
// }

inline int strcmp(const char * cs,const char * ct)
{
register int __res __asm__("ax");
__asm__("cld\n"
	"1:\tlodsb\n\t"
	"scasb\n\t"
	"jne 2f\n\t"
	"testb %%al,%%al\n\t"
	"jne 1b\n\t"
	"xorl %%eax,%%eax\n\t"
	"jmp 3f\n"
	"2:\tmovl $1,%%eax\n\t"
	"jb 3f\n\t"
	"negl %%eax\n"
	"3:"
	:"=a" (__res):"D" (cs),"S" (ct):"dx","cx");
return __res;
}

inline int strncmp(const char * cs,const char * ct,size_t count)
{
register int __res __asm__("ax");

__asm__("cld\n"
	"1:\tdecl %3\n\t"
	"js 2f\n\t"
	"lodsb\n\t"
	"scasb\n\t"
	"jne 3f\n\t"
	"testb %%al,%%al\n\t"
	"jne 1b\n"
	"2:\txorl %%eax,%%eax\n\t"
	"jmp 4f\n"
	"3:\tmovl $1,%%eax\n\t"
	"jb 4f\n\t"
	"negl %%eax\n\t"
	"4:"
	:"=a" (__res):"D" (cs),"S" (ct),"c" (count):"dx");
return __res;
}

inline char * strchr(const char * s,char c)
{
register char * __res __asm__("ax");
__asm__("cld\n\t"
	"movb %%al,%%ah\n"
	"1:\tlodsb\n\t"
	"cmpb %%ah,%%al\n\t"
	"je 2f\n\t"
	"testb %%al,%%al\n\t"
	"jne 1b\n\t"
	"movl $1,%1\n"
	"2:\tmovl %1,%0\n\t"
	"decl %0"
	:"=a" (__res):"S" (s),"0" (c));
return __res;
}

inline char * strrchr(const char * s,char c)
{
register char * __res __asm__("dx");
__asm__("pushl %%eax\n\t"
	"pushl %%esi":);
__asm__("cld\n\t"
	"movb %%al,%%ah\n"
	"1:\tlodsb\n\t"
	"cmpb %%ah,%%al\n\t"
	"jne 2f\n\t"
	"movl %%esi,%0\n\t"
	"decl %0\n"
	"2:\ttestb %%al,%%al\n\t"
	"jne 1b\n\t"
	"popl %%esi\n\t"
	"popl %%eax"
	:"=d" (__res):"0" (0),"S" (s),"a" (c));
return __res;
}

inline size_t strspn(const char * cs, const char * ct)
{
register char * __res __asm__("si");

__asm__("cld\n\t"
	"movl %4,%%edi\n\t"
	"repne\n\t"
	"scasb\n\t"
	"notl %%ecx\n\t"
	"decl %%ecx\n\t"
	"movl %%ecx,%%edx\n"
	"1:\tlodsb\n\t"
	"testb %%al,%%al\n\t"
	"je 2f\n\t"
	"movl %4,%%edi\n\t"
	"movl %%edx,%%ecx\n\t"
	"repne\n\t"
	"scasb\n\t"
	"je 1b\n\t"
	"2:\tdecl %0"
	:"=S" (__res):"a" (0),"c" (0xffffffff),"0" (cs),"g" (ct));
return __res-cs;
}

inline size_t strcspn(const char * cs, const char * ct)
{
register char * __res __asm__("si");
__asm__("pushl %%eax\n\t"
	"pushl %%ecx":);
__asm__("cld\n\t"
	"movl %4,%%edi\n\t"
	"repne\n\t"
	"scasb\n\t"
	"notl %%ecx\n\t"
	"decl %%ecx\n\t"
	"movl %%ecx,%%edx\n"
	"1:\tlodsb\n\t"
	"testb %%al,%%al\n\t"
	"je 2f\n\t"
	"movl %4,%%edi\n\t"
	"movl %%edx,%%ecx\n\t"
	"repne\n\t"
	"scasb\n\t"
	"jne 1b\n"
	"2:\tdecl %0"
	:"=S" (__res):"a" (0),"c" (0xffffffff),"0" (cs),"g" (ct)
:"dx","di");
__asm__("popl %%ecx\n\t"
	"popl %%eax":);
return __res-cs;
}

inline char * strpbrk(const char * cs,const char * ct)
{
register char * __res __asm__("si");
__asm__("pushl %%eax\n\t"
	"pushl %%ecx":);
__asm__("cld\n\t"
	"movl %4,%%edi\n\t"
	"repne\n\t"
	"scasb\n\t"
	"notl %%ecx\n\t"
	"decl %%ecx\n\t"
	"movl %%ecx,%%edx\n"
	"1:\tlodsb\n\t"
	"testb %%al,%%al\n\t"
	"je 2f\n\t"
	"movl %4,%%edi\n\t"
	"movl %%edx,%%ecx\n\t"
	"repne\n\t"
	"scasb\n\t"
	"jne 1b\n\t"
	"decl %0\n\t"
	"jmp 3f\n"
	"2:\txorl %0,%0\n"
	"3:"
	:"=S" (__res):"a" (0),"c" (0xffffffff),"0" (cs),"g" (ct)
:"dx","di");
__asm__("popl %%ecx\n\t"
	"popl %%eax":);
return __res;
}

inline char * strstr(const char * cs,const char * ct)
{
register char * __res __asm__("ax");
__asm__("pushl %%ecx\n\t"
	"pushl %%esi":);
__asm__("cld\n\t" \
	"movl %4,%%edi\n\t"
	"repne\n\t"
	"scasb\n\t"
	"notl %%ecx\n\t"
	"decl %%ecx\n\t"	/* NOTE! This also sets Z if searchstring='' */
	"movl %%ecx,%%edx\n"
	"1:\tmovl %4,%%edi\n\t"
	"movl %%esi,%%eax\n\t"
	"movl %%edx,%%ecx\n\t"
	"repe\n\t"
	"cmpsb\n\t"
	"je 2f\n\t"		/* also works for empty string, see above */
	"xchgl %%eax,%%esi\n\t"
	"incl %%esi\n\t"
	"cmpb $0,-1(%%eax)\n\t"
	"jne 1b\n\t"
	"xorl %%eax,%%eax\n\t"
	"2:"
	:"=a" (__res):"0" (0),"c" (0xffffffff),"S" (cs),"g" (ct)
:"dx","di");
__asm__("popl %%esi\n\t"
	"popl %%ecx":);
return __res;
}

inline size_t strlen(const char * s)
{
register int __res __asm__("cx");
__asm__("cld\n\t"
	"repne\n\t"
	"scasb\n\t"
	"notl %0\n\t"
	"decl %0"
	:"=c" (__res):"D" (s),"a" (0),"0" (0xffffffff));
return __res;
}

char * ___strtok;

inline char * strtok(char * s,const char * ct)
{
register char * __res;
__asm__("testl %1,%1\n\t"
	"jne 1f\n\t"
	"testl %0,%0\n\t"
	"je 8f\n\t"
	"movl %0,%1\n"
	"1:\txorl %0,%0\n\t"
	"movl $-1,%%ecx\n\t"
	"xorl %%eax,%%eax\n\t"
	"cld\n\t"
	"movl %4,%%edi\n\t"
	"repne\n\t"
	"scasb\n\t"
	"notl %%ecx\n\t"
	"decl %%ecx\n\t"
	"je 7f\n\t"			/* empty delimeter-string */
	"movl %%ecx,%%edx\n"
	"2:\tlodsb\n\t"
	"testb %%al,%%al\n\t"
	"je 7f\n\t"
	"movl %4,%%edi\n\t"
	"movl %%edx,%%ecx\n\t"
	"repne\n\t"
	"scasb\n\t"
	"je 2b\n\t"
	"decl %1\n\t"
	"cmpb $0,(%1)\n\t"
	"je 7f\n\t"
	"movl %1,%0\n"
	"3:\tlodsb\n\t"
	"testb %%al,%%al\n\t"
	"je 5f\n\t"
	"movl %4,%%edi\n\t"
	"movl %%edx,%%ecx\n\t"
	"repne\n\t"
	"scasb\n\t"
	"jne 3b\n\t"
	"decl %1\n\t"
	"cmpb $0,(%1)\n\t"
	"je 5f\n\t"
	"movb $0,(%1)\n\t"
	"incl %1\n\t"
	"jmp 6f\n"
	"5:\txorl %1,%1\n"
	"6:\tcmpb $0,(%0)\n\t"
	"jne 7f\n\t"
	"xorl %0,%0\n"
	"7:\ttestl %0,%0\n\t"
	"jne 8f\n\t"
	"movl %0,%1\n"
	"8:"
	:"=b" (__res),"=S" (___strtok)
	:"0" (___strtok),"1" (s),"g" (ct)
	:"ax","cx","dx","di","memory");
return __res;
}

// inline void * memcpy(void * to, const void * from, size_t n)
// {
// __asm__("pushl %%edi\n\t"
// 	"pushl %%esi":);
// __asm__("cld\n\t"
// 	"movl %%edx, %%ecx\n\t"
// 	"shrl $2,%%ecx\n\t"
// 	"rep ; movsl\n\t"
// 	"testb $1,%%dl\n\t"
// 	"je 1f\n\t"
// 	"movsb\n"
// 	"1:\ttestb $2,%%dl\n\t"
// 	"je 2f\n\t"
// 	"movsw\n"
// 	"2:\n"
// 	: /* no output */
// 	:"d" (n),"D" ((long) to),"S" ((long) from)
// : "cx","memory");
// __asm__("popl %%esi\n\t"
// 	"popl %%edi":);
// return (to);
// }

inline void * memcpy(void * to, const void * from, size_t n)
{
__asm__("cld\n\t"
	"movl %%edx, %%ecx\n\t"
	"shrl $2,%%ecx\n\t"
	"rep ; movsl\n\t"
	"testb $1,%%dl\n\t"
	"je 1f\n\t"
	"movsb\n"
	"1:\ttestb $2,%%dl\n\t"
	"je 2f\n\t"
	"movsw\n"
	"2:\n"
	: /* no output */
	:"d" (n),"D" ((long) to),"S" ((long) from)
	: "cx","memory");
return (to);
}

inline void * memmove(void * dest,const void * src, size_t n)
{
__asm__("pushl %%ecx\n\t"
	"pushl %%esi\n\t"
	"pushl %%edi":);

if (dest<src)
__asm__("cld\n\t"
	"rep\n\t"
	"movsb"
	: /* no output */
	:"c" (n),"S" (src),"D" (dest)
	:);
else
__asm__("std\n\t"
	"rep\n\t"
	"movsb\n\t"
	"cld"
	: /* no output */
	:"c" (n),
	"S" (n-1+(const char *)src),
	"D" (n-1+(char *)dest)
	:"memory");

__asm__("popl %%edi\n\t"
	"popl %%esi\n\t"
	"popl %%ecx":);
return dest;
}

inline int memcmp(const void * cs,const void * ct,size_t count)
{
register int __res __asm__("ax");
__asm__("pushl %%esi\n\t"
	"pushl %%edi\n\t"
	"pushl %%ecx":);
__asm__("cld\n\t"
	"repe\n\t"
	"cmpsb\n\t"
	"je 1f\n\t"
	"movl $1,%%eax\n\t"
	"jb 1f\n\t"
	"negl %%eax\n"
	"1:"
	:"=a" (__res):"0" (0),"D" (cs),"S" (ct),"c" (count)
	:);
__asm__("popl %%ecx\n\t"
	"popl %%edi\n\t"
	"popl %%esi":);
return __res;
}

extern inline void * memchr(const void * cs,char c,size_t count)
{
register void * __res __asm__("di");
if (!count)
	return NULL;
__asm__("pushl %%ecx":);
__asm__("cld\n\t"
	"repne\n\t"
	"scasb\n\t"
	"je 1f\n\t"
	"movl $1,%0\n"
	"1:\tdecl %0"
	:"=D" (__res):"a" (c),"D" (cs),"c" (count)
	:);
__asm__("popl %%ecx":);
return __res;
}

inline void * memset(void * s,char c,size_t count)
{
__asm__("cld\n\t"
	"rep\n\t"
	"stosb"
	: /* no output */
	:"a" (c),"D" (s),"c" (count)
	:"memory");
return s;
}

// extern inline void * memset(void * s,char c,size_t count)
// {
// __asm__("pushl %%ecx\n\t"
// 	"pushl %%edi":);
// __asm__("cld\n\t"
// 	"rep\n\t"
// 	"stosb"
// 	: /* no output */
// 	:"a" (c),"D" (s),"c" (count)
// 	:"memory");
// __asm__("popl %%edi\n\t"
// 	"popl %%ecx":);
// return s;
// }

