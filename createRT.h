#ifndef __CREATE_RT__
#define __CREATE_RT__

#include <stdint.h>

#define RAINBOW_SIZE 1000000
#define NUM_OF_TABLES 2
#define CHAIN_LEN 1500

static const char basis_64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@";

uint8_t* rand_string(int);
unsigned long djb2(char *, size_t);
char* reduce(uint8_t *, size_t);
void fprint_hash(FILE *, const uint8_t *, int);
uint8_t* create_chain(const uint8_t *, size_t);
void create_RT(size_t, size_t, size_t);

#endif