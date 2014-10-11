/**
 *
 * tivi: A tiny VI clone.
 *
 * See LICENSE file for license information.
 *
 * Copyright (c) 2000 Neeraj Sharma <neeraj.sharma@alumni.iitg.ernet.in>
 */

#include "editor9.h"

#include <cstring>
#include <curses.h>

Buf *
get_buffer(void) {
  Buf *b;

  b = new Buf;
  b->no_of_lines = 1;
  b->screen_start_line = 1;
  b->y = 1;
  b->filename = 0;
  b->currrow = get_rownode();
  return (b);
}

Row *
get_rownode(void) {
  Row *r;

  r = new Row;
  r->leftptr = 0;
  r->rightptr = 0;
  r->currlnode = 0;
  r->x = 1;
  r->no_of_chars = 0;
  return r;
}

Lnode *
get_lnode(void) {
  Lnode *l;

  l = new Lnode;
  l->leftptr = 0;
  l->rightptr = 0;
  return l;
}

int
delete_buffer(Buf * &b) {
  if (b == 0)
    return (ERROR);
  while (delete_line(b) != ERROR);
  delete b->currrow;
  delete b;
  b = 0;
  return 0;
}

int
screen_pos(Row * r) {
  Lnode *l;
  int i;
  int count = 0;
  char ch;

  l = r->currlnode;
  if (l == 0)
    return (1);

  while (l->leftptr != 0)
    l = l->leftptr;
  i = 1;
  while (i < r->x && l != 0) {
    if (i % 16 == 0)
      ch = l->chs[15];
    else
      ch = l->chs[(i % 16) - 1];
    if (ch == TAB_MYKEY)        // tab key
      count += TAB_VAL;
    else
      count++;
    if (i % 16 == 0)
      l = l->rightptr;
    i++;
  }
  count++;                      // counting the present character itself
  return (count);
}

int
screen_line_size(Row * r) {
  int count = 0, i;
  char ch;
  Lnode *l;
  if (r->currlnode == 0)
    return 0;
  else {
    i = 1;
    l = r->currlnode;
    while (l->leftptr != 0)
      l = l->leftptr;
    while (i <= r->no_of_chars && l != 0) {
      if (i % 16 == 0)
        ch = l->chs[15];
      else
        ch = l->chs[(i % 16) - 1];
      if (ch == TAB_MYKEY)      // tab key
        count += TAB_VAL;
      else
        count++;
      if (i % 16 == 0)
        l = l->rightptr;
      i++;
    }
  }
  return (count);
}

int
get_node_no(int pos) {
  int n = 0;

  if (pos != 0 && pos % 16 == 0)
    n = (pos / 16) - 1;
  else
    n = pos / 16;
  return (n);
}

int
gox(Buf * b, int pos) {
  Row *r;
  int n = 0, n2 = 0;

  r = b->currrow;
  if (pos == r->no_of_chars + 1) {
    if (r->currlnode != 0)
      while (r->currlnode->rightptr != 0)
        r->currlnode = r->currlnode->rightptr;

    r->x = pos;
  } else if (pos > (r->no_of_chars + 1) || pos <= 0)
    return (ERROR);
  else {
    n = get_node_no(r->x);
    n2 = get_node_no(pos);
    n = n - n2;
    while (n != 0) {
      if (n < 0) {
        r->currlnode = r->currlnode->rightptr;
        n++;
      } else if (n > 0) {
        r->currlnode = r->currlnode->leftptr;
        n--;
      }
    }                           // end of while
  }
  r->x = pos;
  return (0);
}

char
getele(Buf * b) {
  char ch;
  int x1;

  x1 = b->currrow->x;
  if (x1 % 16 == 0)
    ch = b->currrow->currlnode->chs[15];
  else
    ch = b->currrow->currlnode->chs[(x1 % 16) - 1];

  return (ch);
}

// dont forget to delete the returned pointer after use
char *
get_buf_line(Row * r) {
  char *s;
  int x1, nc;
  int j, count, lsize;
  Lnode *l, *l2;
  char ch;

  l = r->currlnode;
  l2 = l;
  nc = r->no_of_chars;
  x1 = 1;
  //if (nc==0)
  // return(0);
  lsize = screen_line_size(r);
  s = new char[lsize + 1];
  if (l != 0)
    while (l->leftptr != 0) {
      l = l->leftptr;
      if (l != 0)
        l2 = l;
    }
  if (l == 0)
    l = l2;
  count = 0;
  while (x1 <= nc) {
    if ((x1 % 16) != 0) {
      ch = l->chs[(x1 % 16) - 1];
      if (ch == TAB_MYKEY)      // for tab
        for (j = 0; j < TAB_VAL; j++)
          s[count++] = ' ';
      else
        s[count++] = ch;
    } else {
      ch = l->chs[15];
      if (ch == TAB_MYKEY)      // tab key
        for (j = 0; j < TAB_VAL; j++)
          s[count++] = ' ';
      else
        s[count++] = ch;
      l = l->rightptr;
    }
    x1++;
  }                             // end of while
  s[count] = '\0';
  return (s);
}

int
putele(Buf * b, char ch) {
  int x1, nc;
  Row *r;
  Lnode *l;

  x1 = b->currrow->x;
  nc = b->currrow->no_of_chars;
  if (x1 < 1 || x1 > nc + 1)
    return (ERROR);
  if (x1 == nc + 1 && nc % 16 == 0 && nc != 0) {
    if (nc == 80 && b->no_of_lines == MAXBUFLEN)
      return (ERROR_FULL);
    if (nc == 80) {
      r = new Row;
      r->leftptr = b->currrow;
      r->rightptr = b->currrow->rightptr;
      b->currrow->rightptr = r;
      b->currrow = r;
      if (r->rightptr != 0)
        r->rightptr->leftptr = r;
      r->currlnode = get_lnode();
      r->no_of_chars = 1;
      r->currlnode->chs[0] = ch;
    } else {
      l = get_lnode();
      l->leftptr = b->currrow->currlnode;
      b->currrow->currlnode->rightptr = l;
      b->currrow->currlnode = l;
      l->chs[0] = ch;
      b->currrow->no_of_chars++;
    }
  } else if (x1 == nc + 1) {
    if (b->currrow->currlnode == 0)  // when nc==0
      b->currrow->currlnode = get_lnode();
    if (x1 % 16 == 0)
      b->currrow->currlnode->chs[15] = ch;
    else
      b->currrow->currlnode->chs[(x1 % 16) - 1] = ch;
    b->currrow->no_of_chars++;
  } else if (x1 <= nc) {
    if (x1 % 16 == 0)
      b->currrow->currlnode->chs[15] = ch;
    else
      b->currrow->currlnode->chs[(x1 % 16) - 1] = ch;
  }

  return (0);
}

int
insertele(Buf * b, char ch) {
  Lnode *l, *n = 0;
  Row *r;
  int x1, nc, i;
  char chnext;

  if (screen_line_size(b->currrow) == 80)
    return (ERROR);
  if (ch == TAB_MYKEY && screen_line_size(b->currrow) + TAB_VAL > 80)
    return (ERROR);

  l = b->currrow->currlnode;
  i = x1 = b->currrow->x;
  nc = b->currrow->no_of_chars;

  if (x1 > nc)
    putele(b, ch);
  else {
    while (i <= nc) {
      if (i % 16 == 0) {
        chnext = l->chs[15];
        l->chs[15] = ch;
        if (l->rightptr == 0) {
          l->rightptr = get_lnode();
          l->rightptr->leftptr = l;
        }
        l = l->rightptr;
      } else {
        chnext = l->chs[(i % 16) - 1];
        l->chs[(i % 16) - 1] = ch;
      }
      i++;
      ch = chnext;
    }                           // end of while
    if (i % 16 == 0) {
      //chnext=l->chs[15];
      l->chs[15] = ch;
    } else {
      //chnext=l->chs[(i%16)-1];
      l->chs[(i % 16) - 1] = ch;
    }
    b->currrow->no_of_chars++;
  }
  return 0;
}

int
deleteele(Buf * b) {
  Lnode *l, *temp;
  int x1, x2, x3, nc;
  char ch, tmp;

  l = b->currrow->currlnode;
  x1 = b->currrow->x;
  x3 = x1;
  nc = b->currrow->no_of_chars;
  if (x1 > nc)
    return (ERROR);
  else if (x1 <= nc) {
    x2 = x1 + 1;
    while (x2 <= nc) {
      gox(b, x2);
      ch = getele(b);
      gox(b, x1);
      putele(b, ch);
      x1++;
      x2++;
    }
    if (x1 % 16 == 1) {
      gox(b, x1);               // go to the end character
      l = b->currrow->currlnode;
      temp = l;
      if (nc == 1) {
        if (temp != 0)
          delete temp;
        b->currrow->x = 1;
        b->currrow->no_of_chars = 0;
        b->currrow->currlnode = 0;
      } else {
        l = b->currrow->currlnode->leftptr;
        l->rightptr = 0;
        b->currrow->currlnode = l;
        b->currrow->x = x1 - 1;
        if (temp != 0)
          delete temp;
        if (x3 == nc)
          gox(b, x3 - 1);
        else
          gox(b, x3);

        b->currrow->no_of_chars--;
      }
    } else {
      if (x3 == nc)
        gox(b, x3 - 1);
      else
        gox(b, x3);
      b->currrow->no_of_chars--;
    }
  }                             // end of if x1<=nc
  return 0;
}

int
delete_line(Buf * b) {
  Lnode *l;
  Row *r;

  if (not_started(b))
    return (ERROR);
  gox(b, 1);
  l = b->currrow->currlnode;
  while (l != 0) {
    b->currrow->currlnode = l->rightptr;
    delete l;
    l = b->currrow->currlnode;
  }

  b->currrow->x = 1;
  b->currrow->no_of_chars = 0;
  r = b->currrow;
  if (r->rightptr == 0 && r->leftptr == 0)
    return (0);
  else if (r->rightptr == 0) {
    b->currrow = r->leftptr;
    b->currrow->rightptr = 0;
    delete r;
    if (b->y == 1)
      b->screen_start_line--;
    else
      b->y--;
    b->no_of_lines--;
  } else {
    if (r->leftptr == 0) {
      b->currrow = r->rightptr;
      b->currrow->leftptr = 0;
      delete r;
      b->no_of_lines--;
    } else {
      b->currrow = r->rightptr;
      b->currrow->leftptr = r->leftptr;
      r->leftptr->rightptr = b->currrow;
      delete r;
      b->no_of_lines--;
    }
  }
  return (0);
}                               // end of function 

int
insert_line_down(Buf * b) {
  Row *r;

  if (b->no_of_lines == MAXBUFLEN)
    return (ERROR_FULL);
  r = get_rownode();
  r->leftptr = b->currrow;
  r->rightptr = b->currrow->rightptr;
  b->currrow->rightptr = r;
  if (r->rightptr != 0)
    r->rightptr->leftptr = r;
  b->currrow = r;
  b->no_of_lines++;
  if (b->y == SCREEN_MAXLIN)
    b->screen_start_line++;
  else
    b->y++;
  return (0);
}

int
insert_line_above(Buf * b) {
  Row *r;

  if (b->no_of_lines == MAXBUFLEN)
    return (ERROR_FULL);
  r = get_rownode();
  r->leftptr = b->currrow->leftptr;
  r->rightptr = b->currrow;
  b->currrow->leftptr = r;
  if (r->leftptr != 0)
    r->leftptr->rightptr = r;
  b->no_of_lines++;
  b->currrow = r;
  return (0);
}

int
not_started(Buf * b) {
  if (b->no_of_lines == 1 && b->currrow->no_of_chars == 0)
    return (1);
  else
    return (0);
}

int
set_filename(Buf * b, const char *fname) {
  if (b == 0)
    return (ERROR);
  if (b->filename != 0)
    return (ERROR);
  else {
    b->filename = new char[strlen(fname) + 1];
    b->filename[0] = '\0';
    strcat(b->filename, fname);
  }
  return (0);
}

int
file_exists(char *fname) {
  FILE *fin;

  fin = fopen(fname, "r");
  if (fin == 0)
    return (0);
  // else  
  fclose(fin);
  return (1);
}

int
write_to_file(Buf * b) {
  Row *r;
  Lnode *l;
  FILE *fout;
  int x1;

  if (b->filename == 0)
    return (ERROR);
  fout = fopen(b->filename, "w");
  if (fout == 0)
    return (ERROR);

  r = b->currrow;
  while (r->leftptr != 0)
    r = r->leftptr;
  while (r != 0) {
    l = r->currlnode;
    if (l != 0) {
      x1 = 1;
      while (l->leftptr != 0)
        l = l->leftptr;
      while (l != 0 && x1 <= r->no_of_chars) {
        if (x1 % 16 == 0) {
          fprintf(fout, "%c", l->chs[15]);
          l = l->rightptr;
        } else
          fprintf(fout, "%c", l->chs[(x1 % 16) - 1]);
        x1++;
      }                         //end of while
    }                           // end of if
    //if (r->rightptr!=0)
    fprintf(fout, "\n");
    r = r->rightptr;
  }                             // end of while r!=0
  fclose(fout);
  return (0);
}

Buf *
read_from_file(const char *fname) {
  Buf *b;
  Row *temp;
  FILE *fin;
  char ch;
  int count;
  int last_endln_flag = 0;

  fin = fopen(fname, "r");
  if (fin == 0)
    return (0);
  b = get_buffer();
  b->filename = new char[strlen(fname) + 1];
  b->filename[0] = '\0';
  strcat(b->filename, fname);
  while (!feof(fin)) {
    fscanf(fin, "%c", &ch);
    if (feof(fin))
      break;
    if (ch != '\n' && ch != '\0') {
      putele(b, ch);
      b->currrow->x++;
      last_endln_flag = 0;
    } else {
      //if (b->currrow->no_of_chars!=0)
      //gox(b,1);
      insert_line_down(b);
      last_endln_flag = 1;
    }
  }                             //end of while
  if (last_endln_flag)
    delete_line(b);

  fclose(fin);
  return (b);
}
