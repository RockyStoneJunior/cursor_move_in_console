#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "curspos.h"
#include "color.h"
#include "key.h"

static struct termios term, oterm;

static int cursor_current_x = MESSAGE_WINDOW_OFFSET_COL;
static int cursor_current_y = MESSAGE_WINDOW_OFFSET_ROW;

static int message_current_x = MESSAGE_WINDOW_OFFSET_COL;
static int message_current_y = MESSAGE_WINDOW_OFFSET_ROW;

static int send_current_x = SEND_WINDOW_OFFSET_COL;
static int send_current_y = SEND_WINDOW_OFFSET_ROW;

static int frilist_current_x = FRILIST_WINDOW_OFFSET_COL;
static int frilist_current_y = FRILIST_WINDOW_OFFSET_ROW;

static int top_border = MESSAGE_TOP_BORDER;
static int bottom_border = MESSAGE_BOTTOM_BORDER;
static int left_border = MESSAGE_LEFT_BORDER;
static int right_border = MESSAGE_RIGHT_BORDER;

static int cursor_region = CURSOR_MESSAGE_REGION;

static char send_buff[360];
static int send_count = 0;

static int getch(void);
static int kbhit(void);
static int kbesc(void);
static int kbget(void);

/*
 * This function is counted read, that is to say it will 
 *      return only if it reads a character from keyboard
 *      buffer.
 */
static int getch(void)
{
	int c = 0;

	tcgetattr(0, &oterm);
	memcpy(&term, &oterm, sizeof(term));
	term.c_lflag &= ~(ICANON | ECHO);
	term.c_cc[VMIN] = 1;
	term.c_cc[VTIME] = 0;
	tcsetattr(0, TCSANOW, &term);
	c = getchar();
	tcsetattr(0, TCSANOW, &oterm);
	return c;
}

/*
 *  This function is timed read. It will return if it doesn't
 *       read a character in 0.1s.
 */
static int kbhit(void)
{
	int c = 0;

	tcgetattr(0, &oterm);
	memcpy(&term, &oterm, sizeof(term));
	term.c_lflag &= ~(ICANON | ECHO);
	term.c_cc[VMIN] = 0;
	term.c_cc[VTIME] = 1;
	tcsetattr(0, TCSANOW, &term);
	c = getchar();
	tcsetattr(0, TCSANOW, &oterm);
	if(c != -1) ungetc(c, stdin);
	return ((c != -1) ? 1 : 0);

}

static int kbesc(void)
{
	int c;

	if(!kbhit()) return KEY_ESCAPE;
	c = getch();
	if(c == '['){
		switch(getch()){
			case 'A':
				c = KEY_UP;
				break;
			case 'B':
				c = KEY_DOWN;
				break;
			case 'C':
				c = KEY_RIGHT;
				break;
			case 'D':
				c = KEY_LEFT;
				break;
			default:
				c = 0;
				break;
		}
	}else if(c == ' '){
		c = KEY_SEND;
	}else{
		c = 0;
	}

	if(c == 0) while (kbhit()) getch();
	return c;
}

static int kbget(void)
{
	int c;

	c = getch();
	return (c == KEY_ESCAPE) ? kbesc() : c;
}

void print_window()
{
	printf(" -----------------------------------------------------------------------------\n");
	printf("|                             By Rocky Stone V1.1                             |\n");
	printf("|-----------------------------------------------------------------------------|\n");
	printf("|                                                            |                |\n");
	printf("|                                                            |                |\n");
	printf("|                                                            |                |\n");
	printf("|                                                            |                |\n");
	printf("|                                                            |                |\n");
	printf("|                                                            |                |\n");
	printf("|                                                            |                |\n");
	printf("|                                                            |                |\n");
	printf("|                                                            |                |\n");
	printf("|                                                            |                |\n");
	printf("|                                                            |                |\n");
	printf("|                                                            |                |\n");
	printf("|                                                            |                |\n");
	printf("|                                                            |                |\n");
	printf("|                                                            |                |\n");
	printf("|                                                            |                |\n");
	printf("|                                                            |                |\n");
	printf("|                                                            |                |\n");
	printf("|                                                            |                |\n");
	printf("|                                                            |                |\n");
	printf("|------------------------------------------------------------|                |\n");
	printf("|                                                            |                |\n");
	printf("|                                                            |                |\n");
	printf("|                                                            |                |\n");
	printf("|                                                            |                |\n");
	printf("|                                                            |                |\n");
	printf("|                                                            |                |\n");
	printf(" -----------------------------------------------------------------------------\n");
}

void cursor_reposition(int col, int row)
{
	cursorforward(col);
	cursordownward(row);
}

void print_message_char(int ch)
{
	if(cursor_current_y <= bottom_border)
	{
		if(ch == '\n')
		{
			cursor_current_x = left_border;
			cursor_current_y++;
			set_curspos(cursor_current_x, cursor_current_y);
		}else{
			putchar(ch);
			cursor_current_x++;

			if(cursor_current_x > right_border)
			{
				cursor_current_x = left_border;
				cursor_current_y++;
				set_curspos(cursor_current_x, cursor_current_y);
			}
		}
	}
}

void print_message(char *msg)
{
	while(*msg)
	{
		print_message_char(*msg);
		msg++;
	}
}

void clear_window(int number)
{
	for(int i = 0; i < number; i++)
	{
		print_message_char(' ');
	}
}

void backspace()
{
	if(cursor_current_y >= top_border)
	{
		cursor_current_x--;

		if(cursor_current_x < left_border)
		{
			cursor_current_x = right_border;
			cursor_current_y--;
		}

		set_curspos(cursor_current_x, cursor_current_y);
		putchar(' ');
		set_curspos(cursor_current_x, cursor_current_y);
	}
}

void print_send_index(int index)
{
	send_current_x = cursor_current_x;
	send_current_y = cursor_current_y;

	cursor_current_x = message_current_x;
	cursor_current_y = message_current_y;

	cursor_region = CURSOR_MESSAGE_REGION;

	set_curspos(cursor_current_x, cursor_current_y);

	top_border = MESSAGE_TOP_BORDER;
	bottom_border = MESSAGE_BOTTOM_BORDER;
	left_border = MESSAGE_LEFT_BORDER;
	right_border = MESSAGE_RIGHT_BORDER;

	printf("%d", index);				

	message_current_x = cursor_current_x;
	message_current_y = cursor_current_y;

	cursor_current_x = send_current_x;
	cursor_current_y = send_current_y;

	cursor_region = CURSOR_SEND_REGION;

	set_curspos(cursor_current_x, cursor_current_y);

	top_border = SEND_TOP_BORDER;
	bottom_border = SEND_BOTTOM_BORDER;
	left_border = SEND_LEFT_BORDER;
	right_border = SEND_RIGHT_BORDER;
}

int main(void)
{
	int c;

	int send_index;

	clear();
	print_window();

	set_curspos(MESSAGE_WINDOW_OFFSET_COL, MESSAGE_WINDOW_OFFSET_ROW);

	char *test_msg = "\x1b[36m\x1b[43mStone: Hello, Rocky!\n\n\x1b[40mRocky: Hello, Stone!\n       This is my first test message! This is my first test message! This is my first test message!\n";
	//print_message(test_msg);

	message_current_x = cursor_current_x;
	message_current_y = cursor_current_y;

	cursor_current_x = send_current_x;
	cursor_current_y = send_current_y;

	cursor_region = CURSOR_SEND_REGION;

	set_curspos(cursor_current_x, cursor_current_y);

	top_border = SEND_TOP_BORDER;
	bottom_border = SEND_BOTTOM_BORDER;
	left_border = SEND_LEFT_BORDER;
	right_border = SEND_RIGHT_BORDER;
	while(1){
		c = kbget();

		if(c == KEY_ESCAPE){
			break;
		}else if(c == KEY_CTRLS){
			//print_message("Ctrl + Space\n");
		}else if(c == KEY_SEND){
			if(send_count > 0)
			{
				send_buff[send_count++] = '\n';
				send_buff[send_count++] = '\n';
				send_buff[send_count] = '\0';

				send_current_x = cursor_current_x;
				send_current_y = cursor_current_y;

				cursor_current_x = message_current_x;
				cursor_current_y = message_current_y;

				cursor_region = CURSOR_MESSAGE_REGION;

				set_curspos(cursor_current_x, cursor_current_y);

				top_border = MESSAGE_TOP_BORDER;
				bottom_border = MESSAGE_BOTTOM_BORDER;
				left_border = MESSAGE_LEFT_BORDER;
				right_border = MESSAGE_RIGHT_BORDER;

				printf(FG_CLR_YEL);
				print_message("Stone: ");
				printf(FG_CLR_CYN);
				print_message(send_buff);
				printf(FG_CLR_WHT);

				message_current_x = cursor_current_x;
				message_current_y = cursor_current_y;

				cursor_current_x = SEND_WINDOW_OFFSET_COL; //send_current_x;
				cursor_current_y = SEND_WINDOW_OFFSET_ROW;//send_current_y;

				cursor_region = CURSOR_SEND_REGION;

				set_curspos(cursor_current_x, cursor_current_y);

				top_border = SEND_TOP_BORDER;
				bottom_border = SEND_BOTTOM_BORDER;
				left_border = SEND_LEFT_BORDER;
				right_border = SEND_RIGHT_BORDER;

				clear_window(send_count - 2);

				cursor_current_x = SEND_WINDOW_OFFSET_COL; //send_current_x;
				cursor_current_y = SEND_WINDOW_OFFSET_ROW;//send_current_y;
				set_curspos(cursor_current_x, cursor_current_y);

				send_count = 0;
			}
		}else if(c == KEY_UP){
				if(cursor_current_y > top_border)
				{
					cursorupward(1);
					cursor_current_y--;
					int send_index = (cursor_current_y - top_border)*60 + (cursor_current_x - left_border);
					print_send_index(send_index);

				}
		}else if(c == KEY_DOWN){
			if(cursor_region == CURSOR_SEND_REGION)
			{
				int send_index = (cursor_current_y - top_border)*60 + (cursor_current_x - left_border);
				if((send_index + 60) <= send_count)
				{
					cursordownward(1);
					cursor_current_y ++;
				}
				
					send_index = (cursor_current_y - top_border)*60 + (cursor_current_x - left_border);
					print_send_index(send_index);

			}else{
				if(cursor_current_y < bottom_border)
				{
					cursordownward(1);
					cursor_current_y ++;
					int send_index = (cursor_current_y - top_border)*60 + (cursor_current_x - left_border);
					print_send_index(send_index);

				}
			}
		}else if(c == KEY_LEFT){
			if(cursor_current_x > left_border)
			{
				cursorbackward(1);
				cursor_current_x--;
				int send_index = (cursor_current_y - top_border)*60 + (cursor_current_x - left_border);
				print_send_index(send_index);

			}
		}else if(c == KEY_RIGHT){
			if(cursor_region == CURSOR_SEND_REGION)
			{
				send_index = (cursor_current_y - top_border)*60 + (cursor_current_x - left_border);

				if(send_index < send_count)
				{
					cursorforward(1);
					cursor_current_x++;
				}

				send_index = (cursor_current_y - top_border)*60 + (cursor_current_x - left_border);
				print_send_index(send_index);
			}else{
				if(cursor_current_x < right_border)
				{
					cursorforward(1);
					cursor_current_x++;
				}
			}
		}else if(c == KEY_CTRLA){
			if(cursor_region == CURSOR_SEND_REGION)
			{
				send_current_x = cursor_current_x;
				send_current_y = cursor_current_y;

				cursor_current_x = message_current_x;
				cursor_current_y = message_current_y;

				cursor_region = CURSOR_MESSAGE_REGION;

				set_curspos(cursor_current_x, cursor_current_y);

				top_border = MESSAGE_TOP_BORDER;
				bottom_border = MESSAGE_BOTTOM_BORDER;
				left_border = MESSAGE_LEFT_BORDER;
				right_border = MESSAGE_RIGHT_BORDER;
			}
		}else if(c == KEY_CTRLB){
			if(cursor_region == CURSOR_MESSAGE_REGION)
			{
				message_current_x = cursor_current_x;
				message_current_y = cursor_current_y;

				cursor_current_x = send_current_x;
				cursor_current_y = send_current_y;

				cursor_region = CURSOR_SEND_REGION;

				set_curspos(cursor_current_x, cursor_current_y);

				top_border = SEND_TOP_BORDER;
				bottom_border = SEND_BOTTOM_BORDER;
				left_border = SEND_LEFT_BORDER;
				right_border = SEND_RIGHT_BORDER;
			}
		}else if(c == KEY_CTRLF){
			if(cursor_region == CURSOR_FRILIST_REGION)
			{
				frilist_current_x = cursor_current_x;
				frilist_current_y = cursor_current_y;

				cursor_current_x = send_current_x;
				cursor_current_y = send_current_y;

				cursor_region = CURSOR_SEND_REGION;

				set_curspos(cursor_current_x, cursor_current_y);

				top_border = SEND_TOP_BORDER;
				bottom_border = SEND_BOTTOM_BORDER;
				left_border = SEND_LEFT_BORDER;
				right_border = SEND_RIGHT_BORDER;
			}
		}else if(c == KEY_CTRLH){
			if(cursor_region == CURSOR_MESSAGE_REGION)
			{
				message_current_x = cursor_current_x;
				message_current_y = cursor_current_y;

				cursor_current_x = frilist_current_x;
				cursor_current_y = frilist_current_y;

				cursor_region = CURSOR_FRILIST_REGION;

				set_curspos(cursor_current_x, cursor_current_y);

				top_border = FRILIST_TOP_BORDER;
				bottom_border = FRILIST_BOTTOM_BORDER;
				left_border = FRILIST_LEFT_BORDER;
				right_border = FRILIST_RIGHT_BORDER;
			}else if(cursor_region == CURSOR_SEND_REGION)
			{
				send_current_x = cursor_current_x;
				send_current_y = cursor_current_y;

				cursor_current_x = frilist_current_x;
				cursor_current_y = frilist_current_y;

				cursor_region = CURSOR_FRILIST_REGION;

				set_curspos(cursor_current_x, cursor_current_y);

				top_border = FRILIST_TOP_BORDER;
				bottom_border = FRILIST_BOTTOM_BORDER;
				left_border = FRILIST_LEFT_BORDER;
				right_border = FRILIST_RIGHT_BORDER;
			}
		}else if(c == KEY_BACKSPACE){
			if(send_count > 0)
			{
				backspace();
				send_count--;
			}
		}else{
			if(cursor_region == CURSOR_SEND_REGION)
			{
				int send_index = (cursor_current_y - top_border)*60 + (cursor_current_x - left_border);
				print_send_index(send_index);

				if(send_index == send_count)
				{
					print_message_char(c);
					send_buff[send_count++] = (char)c;
				}
			}
		}
	}

	printf("\n");
	return 0;
}
