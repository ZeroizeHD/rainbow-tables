#include <stdio.h>
#include <stdlib.h>
#include <openssl/sha.h>
#include "blake.h"
#include "createRT.h"

void Base64encode(char *encoded, const char *string, int len) {
    int i;
    char *p = encoded;
    for (i = 0; i < len - 2; i += 3) {
        *p++ = basis_64[(string[i] >> 2) & 0x3F];
        *p++ = basis_64[((string[i] & 0x3) << 4) | ((int) (string[i + 1] & 0xF0) >> 4)];
        *p++ = basis_64[((string[i + 1] & 0xF) << 2) | ((int) (string[i + 2] & 0xC0) >> 6)];
        *p++ = basis_64[string[i + 2] & 0x3F];
    }
    if (i < len) {
        *p++ = basis_64[(string[i] >> 2) & 0x3F];
        if (i == (len - 1)) {
            *p++ = basis_64[((string[i] & 0x3) << 4)];
            *p++ = '=';
        } else {
            *p++ = basis_64[((string[i] & 0x3) << 4) | ((int) (string[i + 1] & 0xF0) >> 4)];
            *p++ = basis_64[((string[i + 1] & 0xF) << 2)];
        }
        *p++ = '=';
    }
    *p++ = '\0';
    encoded[len] = '\0';
}

uint8_t* rand_string(int size) {
    int i;
    uint8_t *str = (uint8_t *) malloc((size+1) * sizeof(uint8_t));
    for (i = 0 ; i < size ; i++)
        str[i] = basis_64[rand() % (sizeof(basis_64)-1)];
    str[size] = '\0';
    return str;
}

unsigned long djb2(char* str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++) != 0) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    return hash;
}

char* reduction(uint8_t* h, size_t step) {
    unsigned long hash = djb2(h);
    unsigned long hashtep = labs(hash + step);  /* avoid overflow with (long) abs */

    char* pass = malloc(7 * sizeof(char));
    int i;
    for (i = 0; i < 6; ++i) {
        hashtep /= 64;
        pass[i] = basis_64[hashtep % 64];
    }    
    pass[6] = '\0';

    return pass;
}

// char* reduction(uint8_t* h, size_t step) {
//     unsigned char sha1hash[SHA_DIGEST_LENGTH];
//     uint8_t *newh = malloc(34 * sizeof(uint8_t));
//     memcpy(newh, h, 32);
//     newh[32] = step;
//     newh[33] = '\0';
//     SHA1(newh, sizeof(newh), sha1hash);
//     free(newh);
//     char* pass = calloc(7, sizeof(char));
//     pass[6] = '\0';
//     Base64encode(pass, sha1hash, 6);
//     return pass;
// }

void fprint_hash(FILE* fp, const uint8_t *h, int len) {
    int i;
    if (fp != NULL) {
        for (i = 0; i < len; i++) {
            fprintf(fp, "%02x", h[i]);
        }
    } else {
        for (i = 0; i < len; i++) {
            printf("%02x", h[i]);
        }
    }
}

uint8_t* create_chain(const uint8_t* initPass, size_t chainLen) {
    size_t i;
    uint8_t *h = (uint8_t *) malloc(32 * sizeof(uint8_t));
    blake256_hash(h, initPass, 6);
    // printf("%s\n", initPass);
    for (i = 1 ; i < chainLen ; i++) {
        char *pass = reduction(h, i);   /* reduction(h) = pass*/
        // printf("%s\n", pass);
        blake256_hash(h, (uint8_t*) pass, 6);      /* blake(pass) = h */
        free(pass);
    }
    return h;
}

void create_RT(size_t rainbow_size, size_t table_id, size_t chain_len) {
    int i;
    char table_name[15];
    sprintf(table_name, "rainbow_%zu.csv", table_id);
    FILE *fp = fopen(table_name, "w");
    if (fp == NULL) { 
    	fprintf(stderr, "Error opening file!\n");
    	exit(EXIT_FAILURE);
    }
    for (i = 0 ; i < rainbow_size ; i++) {
        uint8_t *str = rand_string(6);
        uint8_t *h = create_chain(str, chain_len); // total of 2048 links

        fprintf(fp, "%s\t", str);
        fprint_hash(fp, h, 32);
        if (i < rainbow_size-1)
        	fprintf(fp, "\n");

        free(str);
        free(h);
    }
    fclose(fp);
}