#pragma once

typedef struct {
    int number;
    char *name;
    char *abbr;
} DARBY_book;

typedef struct {
    int book;
    int chapter;
    int verse;
    char *text;
} DARBY_verse;

extern DARBY_verse DARBY_verses[];

extern int DARBY_verses_length;

extern DARBY_book DARBY_books[];

extern int DARBY_books_length;
