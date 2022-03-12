#ifndef TOKENS_H
#define TOKENS_H

#include "svec.h"

svec* tokenize(const char* text);
int isop(char c);
char* read_non_op(const char* text, int ii);
char* read_op(const char* text, int ii);
svec* reverse(svec* xs);

#endif
