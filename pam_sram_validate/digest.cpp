#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/sha.h>

extern "C"
{
    char *digest(const char *message[]) {

        unsigned char hash[SHA256_DIGEST_LENGTH];

        SHA256_CTX sha256;
        SHA256_Init(&sha256);
        for (int i=0; message[i]; i++) {
            SHA256_Update(&sha256,  message[i], strnlen( message[i], 1024));
        }
        SHA256_Final(hash, &sha256);    

        char *result = (char*)malloc(((sizeof(char) * SHA256_DIGEST_LENGTH) * 2) +1);
        *result = '\0';

        for (int i = 0; i != SHA256_DIGEST_LENGTH; i++) {
            char *s = (result+(i*2));
            snprintf(s, 2+1,  "%02x", (unsigned int)hash[i]);
        }
    
        return result;
    }
}