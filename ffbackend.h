#ifndef _FFBACKEND_H
#define _FFBACKEND_H

void ffbackend_begin();
void ffbackend_end();
size_t ffbackend_get_stock(char *pcode, size_t code_len, char *out, size_t out_len);

#endif
