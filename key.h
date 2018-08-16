#ifndef __KEY_H__
#define __KEY_H__

#define cursorupward(x) printf("\033[%dA", (x))
#define cursordownward(x) printf("\033[%dB", (x))

#define cursorforward(x) printf("\033[%dC", (x))
#define cursorbackward(x) printf("\033[%dD", (x))

#define clear() printf("\033[H\033[2J")
#define set_curspos(x, y) printf("\033[%d;%dH",(y) ,(x))

#define KEY_ESCAPE 	0x001b
#define KEY_ENTER  	0x000a
#define KEY_BACKSPACE 	0x007f
#define KEY_UP	   	0x0105
#define KEY_DOWN   	0x0106
#define KEY_LEFT   	0x0107
#define KEY_RIGHT  	0x0108
#define KEY_SEND   	0x0109

#define KEY_CTRLS  	0x0000
#define KEY_CTRLA  	0x0001
#define KEY_CTRLB  	0x0002
#define KEY_CTRLF  	0x0006
#define KEY_CTRLH  	0x0008

#define CURSOR_MESSAGE_REGION 0
#define CURSOR_SEND_REGION 1
#define CURSOR_FRILIST_REGION 2

#endif
