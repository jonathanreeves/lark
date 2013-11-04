#include <lib/string.h>
#include <lark/video.h>
#include <stdarg.h>

struct console {
	unsigned char attr;
	unsigned char state;
	unsigned int height;
	unsigned int width;
	unsigned int row;
	unsigned int col;
	char *vid;
} lark_console;

/* in case we want to print somewhere besides the console */
static char logbuf[LOG_BUF_SIZE];

void clear_screen(void)
{
	char *vid = lark_console.vid;
	unsigned char attr = lark_console.attr;
	unsigned int i=0;
	while(i < LINE_SIZE*25){
		vid[i] = ' ';
		i++;
		vid[i] = attr;
		i++;
	}

}

static void clear_line(row)
{
	unsigned char *vid = lark_console.vid;
	unsigned char attr = lark_console.attr;
	int i = 0;
/* FIXME: implement a memset function in assembly */
	while(i < LINE_SIZE){
		vid[LINE_SIZE*row + i++] = ' ';
		vid[LINE_SIZE*row + i++] = attr;
	}
}

extern int vsprintf(char *buf, const char *fmt, va_list args);

void printk(const char *message, ...)
{
	char *vid = lark_console.vid;
	unsigned char attr = lark_console.attr;
	unsigned int row = lark_console.row;
	unsigned int col = lark_console.col;
	va_list args;
	char *buf = &logbuf[0];
	int i;

	va_start(args, message);
	i = vsprintf(logbuf, message, args);
	va_end(args);

	/* it's important to keep in mind that col is in bytes but for
	 * each character we have 2 bytes. One for the character, and
	 * one for its attributes
	 */	
	while(*buf != 0) {
		/* FIXME: add tab support */
		if(*buf == '\n') {
			row++;
			col = 0;
			*buf++;
		} else if(*buf == '\r') {
		       	col = 0;
			*buf++;
		} else {
			vid[row*LINE_SIZE + col] = *buf;
			*buf++;
			col++;
			vid[row*LINE_SIZE + col] = attr;
			col++;
		}

		if(col >= lark_console.width){
			row++;
			col = 0;
		}
		if(row >= lark_console.height){
			scroll();
			row--;
		}
                
	}
	
	lark_console.row = row;
	lark_console.col = col;
}

void scroll(void)
{
	char *vdst = lark_console.vid;
	int i;
	memcpy(vdst, vdst+LINE_SIZE, LINE_SIZE*24);
	for(i = LINE_SIZE*24; i < LINE_SIZE*25; i+=2){
		vdst[i] = ' ';
		vdst[i+1] = lark_console.attr;
	}
}

void video_set_attr(unsigned char fg, unsigned char bg)
{
	lark_console.attr = (bg<<4) | fg;
}

void video_init(void)
{
	lark_console.attr = VIDEO_WHITE;
	lark_console.vid = (char *)VIDEO_MEM;
	lark_console.height = 25;
	lark_console.width = 80*2; /* 80 character each with an attribute */
	lark_console.state = 0;
	lark_console.row = 0;
	lark_console.col = 0;

	clear_screen();

}
