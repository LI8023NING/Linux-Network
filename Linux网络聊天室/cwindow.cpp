#include"cwindow.h"

cwindow::cwindow()
{
    initscr();
    header = NULL;
    output = NULL;
    flist = NULL;
    input = NULL;
}

void cwindow::create_header()
{
    int _h = LINES/5;
    int _w = COLS;
    int _y = 0;
    int _x = 0;
    header = newwin(_h,_w,_y,_x);
}

void cwindow::create_output()
{
    int _h = (LINES*3)/5;
    int _w = (COLS*3)/4;
    int _y = LINES/5;
    int _x = 0;
    output = newwin(_h,_w,_y,_x);
}

void cwindow::create_flist()
{
    int _h = (LINES*3)/5;
    int _w = COLS/4;
    int _y = LINES/5;
    int _x = (COLS*3)/4;
    flist = newwin(_h,_w,_y,_x);
}

void cwindow::create_input()
{
    int _h = LINES/5;
    int _w = COLS;
    int _y = (LINES*4)/5;
    int _x = 0;
    input = newwin(_h,_w,_y,_x);
}

void cwindow::myflush(WINDOW *_win)
{
    box(_win,0,0);
    wrefresh(_win);
}

void cwindow::put_str_to_win(WINDOW *win,int _y,int _x,std::string &str)
{
    mvwaddstr(win,_y,_x,str.c_str());
}

cwindow::~cwindow()
{
    endwin();
}


int main()
{
    cwindow win;
    win.create_header();
    sleep(2);
    win.myflush(win.header);
    sleep(2);

    win.create_output();
    sleep(2);
    win.myflush(win.output);
    sleep(2);

    win.create_flist();
    sleep(2);
    win.myflush(win.flist);
    sleep(2);

    win.create_input();
    std::string msg = "Please Enter# ";
    win.put_str_to_win(win.input,1,2,msg);
    sleep(2);
    win.myflush(win.input);
    sleep(2);

    return 0;
}















