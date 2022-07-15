#ifndef _API_H
#define _API_H

#ifdef __cplusplus
extern "C" {
#endif

char * api(const char* url, const char *method, char *headers[], const char* data);

#ifdef __cplusplus
}
#endif

#endif // _API_H
