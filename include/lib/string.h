
#ifndef __STRING_H__
#define __STRING_H__

static inline void *__memcpy(void *dst, const void *src, unsigned int count)
{
	__asm__ __volatile__ (
		"rep; movsl\n\t"
		"cld\n\t"
		"testb $2, %b3\n\t"
		"je 1f\n"
		"movsw\n\t"
		"1:\ttestb $1, %b3\n\t"
		"je 2f\n\t"
		"movsb\n"
	        "2:\t\n"	
		: /* no output */
		: "D" (dst), "S" (src), "c" (count>>2), "r" (count)
		: "memory"
		);
	return dst;
}

static inline unsigned int __strnlen(const char *s, unsigned int count)
{
int d0;
register int __res;
__asm__ __volatile__(
	"movl %2,%0\n\t"
	"jmp 2f\n"
	"1:\tcmpb $0,(%0)\n\t"
	"je 3f\n\t"
	"incl %0\n"
	"2:\tdecl %1\n\t"
	"cmpl $-1,%1\n\t"
	"jne 1b\n"
	"3:\tsubl %2,%0\n\t"
	:"=a" (__res), "=&d" (d0)
	:"c" (s),"1" (count));
	return __res;
}

void *memset(void *s, int c, unsigned int n); 
void *memcpy(void *dst, const void *src, unsigned int n);
void *memmove(void *dst, const void *src, unsigned int n);
int memcmp(const void *s1, const void *s2, unsigned int n);
unsigned int strlen(const char* s);
unsigned int strnlen(const char *s, unsigned int maxlen);
int strcmp(const char* s1, const char* s2);
int strncmp(const char* s1, const char* s2, unsigned int limit);
char *strcat(char *s1, const char *s2);
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, unsigned int limit);
char *strdup(const char *s1);
int atoi(const char *buf);
char *strchr(const char *s, int c);
char *strrchr(const char *s, int c);
char *strpbrk(const char *s, const char *accept);

int sprintf(char * buf, const char *fmt, ...);

#endif  /* STRING_H */
