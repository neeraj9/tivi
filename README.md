tivi
======

> A tiny VI clone.

Motivation
------------

I started this project as a college assignment and came up with a tiny text
editor in a week. The activity was both tiring and inspiring because I could
learn a bunch of new concepts (curses, c++, datastructures) all at the same time
much better than any textbook could explain.

After so many years I want to resurrect this source to fix the source and
add some very useful features yet keep this binary small, for use in embedded
systems along with educational research.


> The c++ and datastructure usage is very primitive due to my inexperience and
> considering the fact that I was just beginning to learn c++ and
> datastructures. I intend to fix that now as and when I get the time.


- Neeraj Sharma

Coding Guidelines
-------------------


The coding guidelines are similar to Google coding guidelines and the files are
processed with GNU Indent to change the most part as follows:

    indent -br -ce -l80 -sob -cdw -cdb -sc -ncs -nbs -saf -sai -saw -nprs \
      -nbc -brs -brf -ppi 2 -nbbo -nut -ts 2 -npcs -v -o tivi-fixed.cpp tivi.cpp

> See http://www.gnu.org/software/indent/manual/indent.html
> See http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
