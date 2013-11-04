#ifndef __VIDEO_H__
#define __VIDEO_H__

#define VIDEO_BLACK 0x0
#define VIDEO_BLUE 0x1
#define VIDEO_GREEN 0x2
#define VIDEO_CYAN 0x3
#define VIDEO_RED 0x4
#define VIDEO_MAGENTA 0x5
#define VIDEO_BROWN 0x6
#define VIDEO_LGREY 0x7
#define VIDEO_DGREY 0x8
#define VIDEO_LBLUE 0x9
#define VIDEO_LGREEN 0xa
#define VIDEO_LCYAN 0xb
#define VIDEO LRED 0xc
#define VIDEO_LMAGENTA 0xd
#define VIDEO_LBROWN 0xe
#define VIDEO_WHITE 0xf


#define VIDEO_MEM 0xb8000
#define LINE_SIZE (80*2)

#define LOG_BUF_SIZE 1024

void video_init(void);
void clear_screen(void);
void video_set_attr(unsigned char fg, unsigned char bg);
void printk(const char *message, ...);
void scroll(void);

#endif
