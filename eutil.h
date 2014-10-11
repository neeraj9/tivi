/**
 *
 * tivi: A tiny VI clone.
 *
 * See LICENSE file for license information.
 *
 * Copyright (c) 2000 Neeraj Sharma <neeraj.sharma@alumni.iitg.ernet.in>
 */

#ifndef EUTIL_H
#define EUTIL_H

#define COMMAND 0
#define TEXT 1
#define INS  2
#define OVRW 3
#define OPEN 4
#define SAVE 5
#define SAVE_AND_EXIT 6
#define CLOSE 7
#define BUFFER_EMPTY 8
#define EXEC_CMD 9
#define TXT_INS   "<< INSERT >>"
#define TXT_OVRW  "<< OVERWRITE >>"
#define CMD       "<< COMMAND >>"

#define ESC_MYKEY   27
#define ENTER_MYKEY 13
#define PAGEUP_MYKEY 300 // set here the correct val.
#define DEL_MYKEY 330
#define PGUP_MYKEY 339
#define PGDOWN_MYKEY 338
#define CTRL_F 6
#define CTRL_B 2
#define CTRL_D 4
#define CTRL_U 21
#define CTRL_K 11
#define CTRL_L 12
#define CTRL_J 10
#define CTRL_H 8

#include "editor9.h"

void start_session(void);	// to start the session
void read_cmd(char *t,char ch);	// to read command from the user
char *get_st(char *st,int n);	// to get a string
char *process_cmd(Buf *buffer,char *st,int &option);
				// to process the command appropriately
void i2st(char *st,int n);
void print_error(const char *s); // print error
void prn_txt(Buf *buffer,int option); // print the details
					//e.g. filename , lines..
int buf_to_screen(Buf *b,int start_line_no); // to print from the buffer to
						//the screen

WINDOW *textwin,*msgwin;
			// msgwin is for the message window
			// textwin is for the window for text editing

#endif // EUTIL_H
