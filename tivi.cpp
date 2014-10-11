/**
 *
 * tivi: A tiny VI clone.
 *
 * See LICENSE file for license information.
 *
 * Copyright (c) 2000 Neeraj Sharma <neeraj.sharma@alumni.iitg.ernet.in>
 */
#include<iostream>
#include<cstdlib>
#include<curses.h>
#include<string.h>

#include "editor9.h"
#include "eutil.h"

using namespace std;

const char *ter_st[1] = { ":q" };

int text_mode(int c);
int common_fn(int c, int mode);
void ins_lin_dn(void);
int cut_lin(void);
void del_lin(void);

Buf *buffer;                    // globel buffer  

int
main(int argc, char *argv[]) {
  int i = 0;
  int choice;                   // used to take the user choice 
  char cmd_line[10];            // used to store the command line
  int exit_flag = 0;            // this flag is set if user want to exit from the editor
  int wrong_cmd_flag = 0;       // this flag is set if user command is not recognised by the editor
  char *str1;
  int op1;                      // it stores the users option   e.g. : SAVE/SAVE AND EXIT ...

  buffer = 0;                // initially buffer is empty
  if (argc > 2)                 // if more than one arguments passed to the editor
  {
    cout << " Syntax Error ! " << endl << " only one filename accepted" << endl;
    return (0);
  } else if (argc == 2) {
    if (!file_exists(argv[1])) {
      cout << " file does not exist !" << endl;
      return (0);
    } else {
      str1 = argv[1];
      buffer = read_from_file(str1);
      start_session();
      refresh();
      buf_to_screen(buffer, 1);
    }
  } else                        // if no arguments passed to the editor
  {
    start_session();

    buffer = 0;
  }

  choice = getch();

  while (!exit_flag) {
    op1 = 0;
    switch (choice) {
    case 'a':
    case 'i':
    case 'o':
    case 'O':
    case 'r':
      text_mode(choice);
      break;
    case ':':
      // this is for the command line
      ungetch(choice);
      read_cmd(cmd_line, choice);
      i = 0;
      while (!exit_flag && i <= 0) {
        if (strcmp(cmd_line, ter_st[i]) == 0)
          exit_flag = 1;
        i++;
      }

      if (!exit_flag) {
        str1 = process_cmd(buffer, cmd_line, op1);
        // ********* error ************
        if (op1 == ERROR) {
          print_error(" Not a valid command ! ");
          wrong_cmd_flag = 1;
        } else
          // **** buffer empty ********
        if (op1 == BUFFER_EMPTY) {
          print_error(" Buffer is empty !! ");
          wrong_cmd_flag = 1;
        } else
          // ******* execute command ***********
        if (op1 == EXEC_CMD) {
          endwin();             // end session
          cout << endl;
          i = system(str1);
          if (i == 127 || i == -1) {
            cout << endl << " Bad shell command ! ";
            cout.flush();
            wrong_cmd_flag = 1;
          }
          cout << endl << " Job executed ....  Press any key to continue";
          cout.flush();
          i = getch();
          start_session();      // start session
          if (buffer != 0)
            buf_to_screen(buffer, buffer->screen_start_line);
        } else
          // *******  SAVE  ***********
        if (op1 == SAVE) {
          if (buffer->filename == 0) {
            if (str1 == 0) {
              print_error(" No name assigned to the present file ! ");
              wrong_cmd_flag = 1;
            } else if (file_exists(str1)) {
              print_error(" File already exists !! ");
              wrong_cmd_flag = 1;
            } else {
              set_filename(buffer, str1);
              write_to_file(buffer);
            }
          } else {
            if (strcmp(str1, buffer->filename) == 0)
              write_to_file(buffer);
            else if (file_exists(str1)) {
              print_error(" File already exists !! ");
              wrong_cmd_flag = 1;
            } else {
              delete buffer->filename;
              buffer->filename = 0;
              set_filename(buffer, str1);
              write_to_file(buffer);
            }
          }
        } else
          //  ********** save and exit ***********
        if (op1 == SAVE_AND_EXIT) {
          if (buffer->filename == 0) {
            print_error(" No name assigned to the present file ! ");
            wrong_cmd_flag = 1;
          } else if (str1 != 0) {
            print_error(" Not a valid command ! ");
            wrong_cmd_flag = 1;
          } else {
            write_to_file(buffer);
            delete_buffer(buffer);
            exit_flag = 1;
          }
        } else
          // *************** close ************
        if (op1 == CLOSE) {

          if (buffer != 0)
            delete_buffer(buffer);
          wclear(textwin);
          wmove(textwin, 0, 0);
          wrefresh(textwin);
          move(0, 0);
          refresh();
          buffer = 0;
        } else
          // *************** open ***************
        if (op1 == OPEN) {
          if (buffer != 0) {
            print_error(" Close present file and then try opening ! ");
            wrong_cmd_flag = 1;
          } else {
            buffer = read_from_file(str1);
            if (buffer != 0) {
              refresh();
              buf_to_screen(buffer, 1);
            } else {
              print_error(" File does not exist !! ");
              wrong_cmd_flag = 1;
            }
          }
        }

      }                         // end of if !exit_flag
      break;
    case 'd':                  // command to delete a line 
      if (buffer == 0) {
        print_error(" Buffer is empty !! ");
        wrong_cmd_flag = 1;
      } else {
        del_lin();
      }
      break;
    case 'x':                  // command to delete a character
      common_fn(DEL_MYKEY, COMMAND);
      wrefresh(textwin);
      break;
    default:
      if ((i = common_fn(choice, COMMAND)), i != 1) {
        if (i == BUFFER_EMPTY)
          print_error(" Buffer is empty !! ");
        else
          print_error(" Not a valid command ! ");
        wrong_cmd_flag = 1;
      }
    }                           //end of switch

    if (!exit_flag) {
      if (!wrong_cmd_flag) {
        if (buffer == 0) {
          wmove(msgwin, 0, 0);
          wdeleteln(msgwin);
          wrefresh(msgwin);
        } else
          prn_txt(buffer, COMMAND);
      }
      if (buffer != 0)
        move(buffer->y - 1, screen_pos(buffer->currrow) - 1);
      refresh();
      choice = getch();
      if (wrong_cmd_flag) {
        //wmove(msgwin,0,0);
        //wdeleteln(msgwin); 
        wclear(msgwin);
        wrefresh(msgwin);
        wrong_cmd_flag = 0;
      }
    }
  }                             // end of while

  endwin();                     // end the session
  return 0;
}                               //end of main

// this function is called when the user wants to edit text
// here c gives the exact option of the user
int
text_mode(int c) {
  int i;
  int ch;
  Row *r;
  int mode;

  if (buffer == 0)
    buffer = get_buffer();
  r = buffer->currrow;
  switch (c) {
  case 'a':
    prn_txt(buffer, INS);
    // if ! empty current line then
    //inc pos in buffer
    if (r->x < r->no_of_chars) {
      gox(buffer, r->x + 1);
      r = buffer->currrow;
      wmove(textwin, buffer->y - 1, screen_pos(r) - 1); //y & x in buffer
    }                           //start from 1 
    else                        //if r->x = r->no_of_chars 
    if (r->x == r->no_of_chars) {
      if (r->x != 80) {
        r->x++;
        wmove(textwin, buffer->y - 1, screen_pos(r) - 1); //y & x in buffer
      } else                    // insert a new line below the current
      {
        ins_lin_dn();
        r = buffer->currrow;
      }
    } else                      // if r->x > r->no_of_chars 
      wmove(textwin, buffer->y - 1, screen_pos(r) - 1);
    mode = INS;
    break;
  case 'i':
    prn_txt(buffer, INS);
    wmove(textwin, buffer->y - 1, screen_pos(r) - 1);
    mode = INS;
    break;
  case 'o':
    prn_txt(buffer, INS);
    //insert a line i (below current) the buffer and make that current 
    ins_lin_dn();
    r = buffer->currrow;
    wmove(textwin, buffer->y - 1, screen_pos(r) - 1);
    mode = INS;
    break;
  case 'r':                    // overwrite mode

    prn_txt(buffer, OVRW);
    wmove(textwin, buffer->y - 1, screen_pos(r) - 1);
    mode = OVRW;
    break;
  case 'O':
    prn_txt(buffer, INS);
    //insert a line i (above current) the buffer and make that current 
    insert_line_above(buffer);
    r = buffer->currrow;
    winsertln(textwin);
    wmove(textwin, buffer->y - 1, screen_pos(r) - 1);
    mode = INS;
    break;
  }                             //end of switch 
  wrefresh(textwin);
  while (1) {
    ch = wgetch(textwin);

    if (!common_fn(ch, TEXT))   // if not arrow or any other non printable key
    {
      r = buffer->currrow;
      switch (ch) {
      case ESC_MYKEY:          // ESC key 
        return (0);
      default:
        if (ch >= 0 && ch <= 27 && ch != TAB_MYKEY)
          break;

        switch (mode) {
        case INS:              // insert mode 

          if (insertele(buffer, ch) == ERROR) {
            beep();
            break;
          }
          if (ch == TAB_MYKEY) {
            for (i = 0; i < TAB_VAL; i++)
              winsch(textwin, ' ');
          } else
            winsch(textwin, ch);
          //wmove(textwin,buffer->y-1,screen_pos(r)-1);
          //if (r->x==80 && ch!=TAB_MYKEY)
          if (screen_pos(r) == 80 && ch != TAB_MYKEY) {
            ins_lin_dn();
            r = buffer->currrow;
          } else {
            r = buffer->currrow;
            if (r->x == r->no_of_chars)
              r->x++;
            else {
              if (r->x % 16 == 0) {
                r->currlnode = r->currlnode->rightptr;
                r->x++;
              } else
                r->x++;
              //gox(buffer,r->x+1);
              //r=buffer->currrow;
            }
            wmove(textwin, buffer->y - 1, screen_pos(r) - 1);
          }
          break;

        case OVRW:             // overwrite mode

          if (ch == TAB_MYKEY) {
            beep();
            break;
          }
          putele(buffer, ch);
          waddch(textwin, ch);
          wmove(textwin, buffer->y - 1, screen_pos(r) - 1);
          if (r->x == 80) {
            ins_lin_dn();
            r = buffer->currrow;
          } else {
            if (r->x == r->no_of_chars)
              r->x++;
            else {
              gox(buffer, r->x + 1);
              r = buffer->currrow;
            }
            wmove(textwin, buffer->y - 1, screen_pos(r) - 1);
          }
          break;
        }                       // end of inner switch
      }                         //end of outer switch

    }
    prn_txt(buffer, mode);
    wrefresh(textwin);
  }                             // end of while

}                               // end of function 

// *************************************************************

// this function handles the arrow and othe keys
// it returns 1 if a function is executed from here and 0 if not
int
common_fn(int c, int mode) {
  int x, y;
  int i;
  char *st;
  Row *r;
  int re_exec_flag = 0;         // flag is set if want to run the while loop again 

  if (buffer == 0)
    return (BUFFER_EMPTY);

  while (1) {
    r = buffer->currrow;
    switch (c) {
    case KEY_LEFT:             //   go left  
    case CTRL_H:
    case 'h':
      if (mode == TEXT && c == 'h')
        return (0);
      if (r->x == 1)
        beep();
      else {
        if (r->x == (r->no_of_chars + 1))
          r->x--;
        else {
          gox(buffer, r->x - 1);
          r = buffer->currrow;
        }
        wmove(textwin, buffer->y - 1, screen_pos(r) - 1);
      }
      re_exec_flag = 0;
      break;

    case KEY_RIGHT:            // go right
    case CTRL_L:
    case 'l':
      if (mode == TEXT && c == 'l')
        return (0);
      if (r->x == r->no_of_chars && mode == COMMAND)
        beep();
      else if (r->x == (r->no_of_chars + 1))  //&& mode==TEXT
        beep();
      else {
        if (mode == COMMAND)
          gox(buffer, r->x + 1);
        else {
          if (r->x == r->no_of_chars)
            r->x++;
          else
            gox(buffer, r->x + 1);
        }
        r = buffer->currrow;
        wmove(textwin, buffer->y - 1, screen_pos(r) - 1);
      }
      re_exec_flag = 0;
      break;

    case KEY_UP:               // go up
    case CTRL_K:
    case 'k':
      if (mode == TEXT && c == 'k')
        return (0);
      if (buffer->screen_start_line == 1 && buffer->y == 1)
        beep();
      else {
        if (buffer->y != 1) {
          r = buffer->currrow->leftptr;
          buffer->currrow = r;
          buffer->y--;
          wmove(textwin, buffer->y - 1, screen_pos(r) - 1);
        } else                  // if buffer->y==1
        {
          buffer->screen_start_line--;
          r = buffer->currrow->leftptr;
          buffer->currrow = r;
          wmove(textwin, 0, 0);
          winsertln(textwin);
          st = get_buf_line(r);
          waddstr(textwin, st);
          delete[]st;
          wmove(textwin, buffer->y - 1, screen_pos(r) - 1);
        }
      }
      re_exec_flag = 0;
      break;

    case KEY_DOWN:             // go down
    case CTRL_J:
    case 'j':
      if (mode == TEXT && c == 'j')
        return (0);
      x = buffer->screen_start_line;
      if (x + buffer->y - 1 == buffer->no_of_lines)
        beep();
      else {
        if ((buffer->y) != SCREEN_MAXLIN) {
          r = buffer->currrow->rightptr;
          buffer->currrow = r;
          buffer->y++;
          wmove(textwin, buffer->y - 1, screen_pos(r) - 1);
        } else                  // if buffer->y==SCREEN_MAXLIN
        {
          buffer->screen_start_line++;
          r = buffer->currrow->rightptr;
          buffer->currrow = r;
          wmove(textwin, 0, 0);
          wdeleteln(textwin);
          st = get_buf_line(r);
          wmove(textwin, SCREEN_MAXLIN - 1, 0);
          waddstr(textwin, st);
          delete[]st;
          wmove(textwin, buffer->y - 1, screen_pos(r) - 1);
        }
      }
      re_exec_flag = 0;
      break;

    case KEY_BACKSPACE:        // backspace

      if (r->x == 1)
        beep();
      else if (r->x > r->no_of_chars) // && mode==TEXT
      {
        r->x--;
        wmove(textwin, buffer->y - 1, screen_pos(r) - 1);
      } else {
        gox(buffer, screen_pos(r) - 1);
        r = buffer->currrow;
        wmove(textwin, buffer->y - 1, screen_pos(r) - 1);
        if (getele(buffer) == TAB_MYKEY) {
          for (i = 0; i < TAB_VAL; i++)
            wdelch(textwin);
        } else
          wdelch(textwin);
        deleteele(buffer);
      }
      re_exec_flag = 0;
      break;

    case ENTER_MYKEY:          // ENTER key

      if (mode == COMMAND)
        re_exec_flag = KEY_DOWN;  // to work this as down arrow
      else {
        cut_lin();
        r = buffer->currrow;
        wmove(textwin, buffer->y - 1, screen_pos(r) - 1);
      }
      break;

    case DEL_MYKEY:            //DEL key

      if (r->x > r->no_of_chars)
        beep();
      else {
        //gox(buffer,r->x-1);
        if (getele(buffer) == TAB_MYKEY) {
          for (i = 0; i < TAB_VAL; i++)
            wdelch(textwin);
        } else
          wdelch(textwin);
        deleteele(buffer);
        r = buffer->currrow;
        wmove(textwin, buffer->y - 1, screen_pos(r) - 1);
      }
      re_exec_flag = 0;
      break;

    case CTRL_F:               // move one page forward 
    case 'f':
      if (mode == TEXT && c == 'f')
        return (0);
      else {
        i = buffer->screen_start_line + SCREEN_MAXLIN - 1;
        if (i < buffer->no_of_lines)
          buf_to_screen(buffer, i + 1);
        else
          beep();
      }
      break;

    case CTRL_B:               // move one page backward
    case 'b':
      if (mode == TEXT && c == 'b')
        return (0);
      else {
        i = buffer->screen_start_line - SCREEN_MAXLIN + 1;
        if (i > 1)
          buf_to_screen(buffer, i - 1);
        else
          beep();
      }
      break;

    case CTRL_D:               // move half page down

      i = buffer->screen_start_line + (SCREEN_MAXLIN / 2) - 1;
      if (i < buffer->no_of_lines)
        buf_to_screen(buffer, i + 1);
      else
        beep();
      break;

    case CTRL_U:               // move half page up
    case 'u':
      if (mode == TEXT && c == 'u')
        return (0);
      else {
        i = buffer->screen_start_line - (SCREEN_MAXLIN / 2) + 1;
        if (i > 1)
          buf_to_screen(buffer, i - 1);
        else
          beep();
      }
      break;

    case '$':                  // go to the end of line

      if (mode == TEXT && c == '$')
        return (0);
      else {
        gox(buffer, buffer->currrow->no_of_chars);
        r = buffer->currrow;
        wmove(textwin, buffer->y - 1, screen_pos(r) - 1);
      }
      break;

    case '^':                  // go to the starting of the line

      if (mode == TEXT && c == '^')
        return (0);
      else {
        gox(buffer, 1);
        r = buffer->currrow;
        wmove(textwin, buffer->y - 1, screen_pos(r) - 1);
      }
      break;

    default:
      return (0);
    }
    if (re_exec_flag == 0) {
      if (mode == COMMAND)
        wrefresh(textwin);
      return (1);
    } else
      c = re_exec_flag;
  }                             // end of while
}

// **************** common_fn() *************************

// this function is for inserting a blank line down the present line

void
ins_lin_dn(void) {
  Row *r;

  r = buffer->currrow;
  if ((buffer->y) <= (SCREEN_MAXLIN - 2)) {
    insert_line_down(buffer);
    r = buffer->currrow;
    wmove(textwin, buffer->y - 1, screen_pos(r) - 1);
    winsertln(textwin);         // insert a line above the current
    wmove(textwin, buffer->y - 1, screen_pos(r) - 1);
  } else if (buffer->y == (SCREEN_MAXLIN - 1)) {
    insert_line_down(buffer);
    r = buffer->currrow;
    wmove(textwin, buffer->y - 1, 0);
    wdeleteln(textwin);
    wmove(textwin, buffer->y - 1, screen_pos(r) - 1);
  } else {
    insert_line_down(buffer);
    r = buffer->currrow;
    wmove(textwin, 0, 0);
    wdeleteln(textwin);
    wmove(textwin, SCREEN_MAXLIN, 0);
  }
}

// **************************************************

// this function cuts the present line from the current cursor position
int
cut_lin(void) {
  Row *r;
  Lnode *l;
  char *st;
  int y1;
  int x1, nc, nc1, i;
  int j;

  if (buffer->no_of_lines == MAXBUFLEN)
    return (ERROR);
  r = buffer->currrow;
  l = r->currlnode;
  x1 = r->x;
  nc = r->no_of_chars;
  y1 = buffer->y;
  wmove(textwin, buffer->y - 1, screen_pos(r) - 1);
  wclrtoeol(textwin);
  ins_lin_dn();
  r = buffer->currrow;
  r = r->leftptr;
  if (x1 <= nc) {
    if (x1 == 1) {
      r->x = 1;
      r->no_of_chars = 0;
      r->currlnode = 0;
      buffer->currrow->currlnode = l;
      buffer->currrow->x = 1;
      buffer->currrow->no_of_chars = nc;
    } else {
      if (x1 % 16 == 1) {
        r->currlnode = r->currlnode->leftptr; // it is illigal only when r->x==1
        r->x--;
        r->currlnode->rightptr = 0;
        r->no_of_chars = x1 - 1;
        l->leftptr = 0;
        buffer->currrow->currlnode = l;
        buffer->currrow->x = 1;
        buffer->currrow->no_of_chars = nc - x1 + 1;
      } else {
        i = x1 % 16;
        if (i == 0)
          i = 16;
        if (l->rightptr == 0) {
          l = get_lnode();
          buffer->currrow->currlnode = l;
          nc1 = nc % 16;
          if (nc1 == 0)
            nc1 = 16;
          j = 0;
          while (i <= nc1)
            l->chs[j++] = r->currlnode->chs[(i++) - 1];
          buffer->currrow->x = 1;
          buffer->currrow->no_of_chars = nc - x1 + 1;
        } else {
          l = l->rightptr;
          l->leftptr = 0;
          buffer->currrow->currlnode = l;
          nc1 = nc % 16;
          buffer->currrow->x = 1;
          buffer->currrow->no_of_chars = nc - (get_node_no(x1) + 1) * 16;
          //if (nc1==0)
          // nc1=16;
          while (i <= 16) {
            insertele(buffer, r->currlnode->chs[(i++) - 1]);
            if (i <= 16)
              buffer->currrow->x++;
          }
          buffer->currrow->x = 1;
        }
        r->x--;
        r->no_of_chars = x1 - 1;
        r->currlnode->rightptr = 0;
      }
    }
  }

  r = buffer->currrow;
  wmove(textwin, buffer->y - 1, screen_pos(r) - 1);
  st = get_buf_line(r);
  waddstr(textwin, st);
  delete[]st;
  wmove(textwin, buffer->y - 1, screen_pos(r) - 1);
  wrefresh(textwin);
  return 0;
}

// **********************************************************

// this function deletes a line
void
del_lin(void) {
  Row *r;
  int lines, y1, sc;
  char *st;

  wdeleteln(textwin);
  y1 = buffer->y;
  sc = buffer->screen_start_line;
  lines = buffer->no_of_lines;
  if (sc + SCREEN_MAXLIN - 1 < lines) // then there are lines(down)apart from that
  {                             // on the screen
    delete_line(buffer);
    r = buffer->currrow;
    while (y1++ < SCREEN_MAXLIN && r->rightptr != 0)
      r = r->rightptr;
    st = get_buf_line(r);
    wmove(textwin, SCREEN_MAXLIN - 1, 0);
    if (st[0] == '\0')
      wclrtoeol(textwin);
    else
      waddstr(textwin, st);
    delete[]st;
    r = buffer->currrow;
    wmove(textwin, buffer->y - 1, screen_pos(r) - 1);
  } else if (sc == lines) {
    delete_line(buffer);
    if (y1 == 1) {
      r = buffer->currrow;
      st = get_buf_line(r);
      wmove(textwin, 0, 0);
      if (st[0] == '\0')
        wclrtoeol(textwin);
      else
        waddstr(textwin, st);
      delete[]st;
      r = buffer->currrow;
      wmove(textwin, buffer->y - 1, screen_pos(r) - 1);
    } else {
      r = buffer->currrow;
      wmove(textwin, buffer->y, screen_pos(r) - 1);
    }
  } else {
    delete_line(buffer);
    r = buffer->currrow;
    wmove(textwin, buffer->y - 1, screen_pos(r) - 1);
  }
  wrefresh(textwin);
}

// *****************************************************
