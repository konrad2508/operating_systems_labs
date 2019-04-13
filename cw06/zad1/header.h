#ifndef HEADER_H
#define HEADER_H

#define BUF_SIZE 1024
#define MAX_CLIENTS 3
#define SERVER_ID 1

enum msgtype{NEW = 1, MIRROR, ADD, MUL, SUB, DIV, TIME, STOP, END};

struct msgbuf {
    long mtype;
    char mtext[BUF_SIZE];
};

#endif
