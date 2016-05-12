#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include "blake.h"
#include "createRT.h"

int main(int argc, char **argv) {
    size_t rainbow_size = (argc == 2) ? atoi(argv[1]) : RAINBOW_SIZE; 
    rainbow_size = (rainbow_size > 0) ? rainbow_size : RAINBOW_SIZE;
    srand((unsigned) time(NULL));
    
    size_t table_id;
    size_t num_of_tables = NUM_OF_TABLES;
    size_t chain_len = CHAIN_LEN;

/* Parallel creation of NUM_OF_TABLES rainbow tables */
#pragma omp parallel for
    for (table_id = 0 ; table_id < num_of_tables ; table_id++) {
        printf("creating %zu of %zu\n", table_id, num_of_tables);
		create_RT(rainbow_size, table_id, chain_len);
    }

    return EXIT_SUCCESS;
}
