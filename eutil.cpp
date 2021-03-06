/**
 *
 * tivi: A tiny VI clone.
 *
 * See LICENSE file for license information.
 *
 * Copyright (c) 2000 Neeraj Sharma <neeraj.sharma@alumni.iitg.ernet.in>
 */

#include "eutil.h"

#include <curses.h>
#include <string.h>

WINDOW *textwin,*msgwin;
			// msgwin is for the message window
			// textwin is for the window for text editing


void start_session(void)
{
 initscr(); 
 cbreak(); 
 noecho();
             nonl();
             intrflush(stdscr, FALSE);
 keypad(stdscr,TRUE);
 textwin=newwin(SCREEN_MAXLIN,80,0,0); //create window for text mode
 keypad(textwin,TRUE);
 msgwin=newwin(1,80,SCREEN_MAXLIN,0);
 //wattrset(msgwin,A_BOLD);
 wclear(msgwin);
 wrefresh(msgwin);
 keypad(msgwin,TRUE);

} 


void read_cmd(char *t,char ch)
{
  //wattrset(msgwin,A_NORMAL);
  wclear(msgwin);
  wrefresh(msgwin);
  wmove(msgwin,0,0);
  echo();
  wgetstr(msgwin,t);
  noecho();
  //wattrset(msgwin,A_BOLD);
}


char *get_st(char *st,int n)
{
 char *s;
 char *s1;

    s1=0;
    s=st+n;
    if (*s==' ')
    {
     while ((*s)==' ')
      s++;
     if (strlen(s)!=0)
     {
      s1=new char[strlen(s)+1];
      strcpy(s1,s);
     }
    }
    return(s1);
}

char *process_cmd(Buf *buffer,char *st,int &option)
{
 //char *s;
 char *s1;

  option=0; 
  s1=0;

   if (strlen(st)==0)
    return(0);
   if (strncmp(st,":o",2)==0)
   {
     s1=get_st(st,2);
     if (s1==0)
      option=ERROR;
     else
      option=OPEN;
     return(s1);
   }
  
   if (strncmp(st,":!",2)==0)
   {
     s1=get_st(st,2);
     if (s1==0)
      option=ERROR;
     else
      option=EXEC_CMD;
     return(s1);
   }
  if (buffer==0)
  { 
   option=BUFFER_EMPTY;
   return(0);
  }
    
  if (strncmp(st,":wq",3) != 0)
  {
   if (strncmp(st,":w",2)==0)
   {
     s1=get_st(st,2);
      option=SAVE;
   }
  }
  else
  {
     s1=get_st(st,3);
     if (s1!=0)
      option=ERROR;
     else
      option=SAVE_AND_EXIT;
  }
  

  if (!option)
  {
   if (strcmp(st,":c")==0)
     option=CLOSE;
   else
     option=ERROR;
  } 

 return(s1);
}



void i2st(char *st,int n)
{
 int i,n1;
 
  n1=n;
  i=0;
  while (n1!=0)
  {
   i++;
   n1=n1/10;
  }
  if (i==0)
  {
    st[0]='0'+(n%10);
    st[1]='\0';
  }
  else
  {
   st[i]='\0';
   while (i>0)
   { 
    st[(i--)-1]='0'+(n%10);
    n=n/10;
   }
  }
}





void print_error(const char *s)	
{
 wclear(msgwin);
 waddstr(msgwin,s);
 wrefresh(msgwin);
}

void prn_txt(Buf *buffer,int option)
{
 int a,b,c;
	   wclear(msgwin);
	   wrefresh(msgwin);  
           if (option==INS)
            waddstr(msgwin,TXT_INS);
           else
           if (option==OVRW)
	    waddstr(msgwin,TXT_OVRW);  
	   else
           if (option==COMMAND)
	    waddstr(msgwin,CMD);  
	   wmove(msgwin,0,15);
           a=screen_pos(buffer->currrow);
	   b=buffer->screen_start_line+buffer->y-1;
	   c=get_node_no(buffer->currrow->x);
          wprintw(msgwin," COL : %-4d LINE : %-4d",a,b);
	  wprintw(msgwin," chars :%-2d",buffer->currrow->no_of_chars); 
	 if (buffer->filename!=0)
          wprintw(msgwin,"  File Name :%s",buffer->filename);
	 else
          wprintw(msgwin,"  File Name : -");
	 wrefresh(msgwin);
}


int buf_to_screen(Buf *b,int start_line_no)
{
 int y;
 int end_line_no;
 int n,line_no;
 Row *r;
 char *st;

  if (b==0)
   return(ERROR);
  if (start_line_no>b->no_of_lines)
   return(ERROR);
  wclear(textwin);
  wrefresh(textwin);
  end_line_no=start_line_no+SCREEN_MAXLIN-1;
  if (end_line_no>b->no_of_lines)
   end_line_no=b->no_of_lines;  
  line_no=b->y;
  n=b->screen_start_line+b->y-1;
  n=n-start_line_no;
  r=b->currrow;
  wclear(textwin);
  wrefresh(textwin);
  while (n!=0)
  {
   if (n<0)
   {
    r=r->rightptr;
    n++;
   }
   else
   if (n>0)
   {
    r=r->leftptr;
    n--;
   }
  } 
  n=start_line_no;
  b->currrow=r;
  b->y=1;
  b->screen_start_line=start_line_no;
  while (b->y < line_no && b->currrow->rightptr!=0)
  {
   b->y++;
   b->currrow=b->currrow->rightptr;
  }
  while (n<=end_line_no && r!=0)
  {
   y=n-start_line_no+1;
   wmove(textwin,y-1,0);
   st=get_buf_line(r); 
   if (st!=0)
   {
    waddstr(textwin,st);
    delete[] st; //  deallocating the space after using it
   }
   r=r->rightptr;
   n++;
  }
  //b->y=n-1;
  //b->screen_start_line=start_line_no;
  wmove(textwin,b->y-1,screen_pos(b->currrow)-1);
  wrefresh(textwin);
  return(0);
}


