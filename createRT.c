#include <stdio.h>
#include <stdlib.h>
#include "blake.h"
#include "createRT.h"

uint8_t* rand_string(int size) {
    int i;
    uint8_t *str = (uint8_t *) malloc((size+1) * sizeof(uint8_t));
    for (i = 0 ; i < size ; i++)
        str[i] = basis_64[rand() % (sizeof(basis_64)-1)];
    str[size] = '\0';
    return str;
}

unsigned long djb2(char* str, size_t len) {
    unsigned long hash = 5381;
    int c;
    while (len--) {
        c = *str++;
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    return hash;
}

void reduce(char* pass, uint8_t* h, size_t step) {
    unsigned long hash = djb2(h, 32);
    int i;
    for (i = 0; i < 6; ++i) {
        hash = (hash / 64) + ((61*step) % 64);
        pass[i] = basis_64[(hash % 64)];
    }
    pass[6] = '\0';
}

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
    uint8_t *h = (uint8_t *) malloc(33 * sizeof(uint8_t));
    h[32] = '\0';
    blake256_hash(h, initPass, 6);
    // printf("%s\n", initPass);
    char *pass = malloc(7 * sizeof(char));
    for (i = 1 ; i < chainLen ; i++) {
        reduce(pass, h, i);   /* reduce(h) = pass*/
        // printf("%s\n", pass);
        blake256_hash(h, (uint8_t*) pass, 6);      /* blake(pass) = h */
    }
    free(pass);
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