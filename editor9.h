/**
 *
 * tivi: A tiny VI clone.
 *
 * See LICENSE file for license information.
 *
 * Copyright (c) 2000 Neeraj Sharma <neeraj.sharma@alumni.iitg.ernet.in>
 */

#define MAXBUFLEN 9000          // maximum line limit put to the buffer
#define ERROR -1
#define ERROR_FULL -2
#define TAB_VAL 8               // this gives the positions moved when TAB is pressed

struct lnode                    // this is the smallest node ie. node of row linked list
{
  struct lnode *leftptr;
  struct lnode *rightptr;
  char chs[16];                 // data part
};

struct row                      // this a node of cloumn linked list ( note that there is only 1 col.)
{
  int x;                        // current position 
  int no_of_chars;
  struct row *leftptr;
  struct row *rightptr;
  struct lnode *currlnode;      // curr node of this row 
};

struct buf                      // this is the main buffer 
{
  int no_of_lines;
  int screen_start_line;        // the line no. where the screen starts
  int y;                        // the curr. y coordinate
  char *filename;               // filename associated with this buffer
  struct row *currrow;          // current row ptr
};

typedef struct lnode Lnode;
typedef struct row Row;
typedef struct buf Buf;

// function prototypes *********************

Buf *get_buffer(void);          // this creates a new buffer 
Row *get_rownode(void);         // this creates a new row node
Lnode *get_lnode(void);         // creates a new lnode
int screen_pos(Row * r);        // gives the screen poition of the (line) row 
        //  pointed by r 
int screen_line_size(Row * r);  // gives the size of the line on the screen
int get_node_no(int pos);       // it returns the node number of the position
int gox(Buf * b, int pos);      // it moves the buffer pointer to the position 
        //  pos in the current line
char getele(Buf * b);           // it returns the element under the cursor
char *get_buf_line(Row * r);    // it returns the line in the buffer pointed
        //  by r
int putele(Buf * b, char ch);   // it overwrites the current char by ch
int insertele(Buf * b, char ch);  // it inserts a char at the current position
int deleteele(Buf * b);         // it deletes the current character
int delete_line(Buf * b);       // it deletes the current line
int insert_line_down(Buf * b);  // it inserts an empty line down the current 
int insert_line_above(Buf * b); // it inserts an empty line above the current
int not_started(Buf * b);       // it returns 1 if not a single char. is 
        //  written in the buffer
int set_filename(Buf * b, const char *fname);
        // this sets the filename for the buffer
int file_exists(char *fname);   // it returns 1 if file exists and 0 if not
int write_to_file(Buf * b);     // it writes everything from the buffer to 
        //  the file
Buf *read_from_file(const char *fname);
        // it generates the buffer from the file
// prototypes end ************************** 
