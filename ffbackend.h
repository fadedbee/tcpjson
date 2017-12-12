#ifndef __FFBACKEND_H_
#define __FFBACKEND_H_

void ffbackend_begin();
void ffbackend_end();
size_t ffbackend_put_stock(char *pcode, size_t pcode_len, char *body, size_t body_len, char *out, size_t out_len);
size_t ffbackend_get_stock(char *pcode, size_t code_len, char *out, size_t out_len);
size_t ffbackend_post_stock(char *pcode, size_t pcode_len, char *body, size_t body_len, char *out, size_t out_len);
size_t ffbackend_delete_stock(char *pcode, size_t pcode_len, char *out, size_t out_len);

#endif
