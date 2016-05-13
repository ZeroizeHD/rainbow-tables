#include <iostream>
#include <fstream>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
    #include "blake.h"
    #include "createRT.h"
#ifdef __cplusplus
}
#endif

using namespace std;

uint8_t* string2uint8_t(string str, size_t len) {
    uint8_t *ret = (uint8_t*) calloc((len/2)+1, sizeof(uint8_t));
    char *t = &str[0];
    for (int i = 0, j = 0 ; i < len ; i++) {
        uint8_t tmp;
        switch (t[i]) {
            case 'a': tmp = 10; break;
            case 'b': tmp = 11; break;
            case 'c': tmp = 12; break;
            case 'd': tmp = 13; break;
            case 'e': tmp = 14; break;
            case 'f': tmp = 15; break;
            default: tmp = t[i] - '0';
        }
        if (i % 2) {
            ret[j] += tmp;
            j++;
        } else {
            ret[j] += tmp*16;
        }
    }
    ret[32] = '\0';
    return ret;
}

string uint8_t2string(uint8_t * h_uint8, size_t len) {
    char *str = (char *) malloc(len * sizeof(char));
    char *strptr = str;
    for (int j = 0; j < len/2; j++) {
        sprintf(strptr, "%02x", h_uint8[j]);
        strptr += 2;
    }
    string h(str);
    free(str);
    return h;
}

char* search_chain(string first_pass_str, size_t chain_len, string hass2find) {
    uint8_t *h_uint8 = (uint8_t *) malloc(32 * sizeof(uint8_t));
    char *pass = (char *) malloc(7 * sizeof(char));
    char *tmp = &first_pass_str[0];
    memcpy(pass, tmp, 7);
    pass[6] = '\0';
    blake256_hash(h_uint8, (uint8_t *) pass, 6);
    string h = uint8_t2string(h_uint8, 64);
    size_t i = 0;
    while (hass2find.compare(h) != 0) {
        free(pass);
        uint8_t *hash = string2uint8_t(h, 64);
        pass = reduction(hash, i);
        blake256_hash(h_uint8, (uint8_t *) pass, 6);
        h = uint8_t2string(h_uint8, 64);
        // cout << pass << "\t"<< h << endl;
        if (i > CHAIN_LEN) {
            fprintf(stderr, "kati malakia egine\n");
            return NULL;
        }
        i++;
    }
    return pass;
};

char* search_RT(string hstr, map<string, string> chain_dict, size_t chain_len) {
    int i = chain_len-1, j = 0;
	string h = hstr;

	while (i > 0) {
		string first_pass = chain_dict[h];

		if (first_pass != "") { /* if found*/ 
            cout << "Searching for " << hstr << " in chain starting with " << first_pass << endl;
            return search_chain(first_pass, chain_len, hstr);
        } else { 
            uint8_t *hash = string2uint8_t(h, 64);
            char *pass = reduction(hash, i);
            // cout << pass << "\t"<< h << endl;

            uint8_t *new_h_uint8 = (uint8_t *) malloc(32 * sizeof(uint8_t));
            blake256_hash(new_h_uint8, (uint8_t*) pass, 6);
            
            h = uint8_t2string(new_h_uint8, 64);
            free(pass);
            // fprint_hash(NULL, new_h_uint8, 32);
            // cout << endl;
        }
        i--;
    }
	return NULL;
}

int main(int argc, char **argv) {
	size_t rainbow_size = (argc == 2) ? atoi(argv[1]) : RAINBOW_SIZE; 
    rainbow_size = (rainbow_size > 0) ? rainbow_size : RAINBOW_SIZE;
    size_t table_id;
    size_t num_of_tables = NUM_OF_TABLES;
    size_t chain_len = CHAIN_LEN;

    // uint8_t* lala = create_chain((uint8_t*)"PL5oET", 24);
    // fprint_hash(NULL, lala, 32);
    // printf("\n");

    string h_str;
    ifstream file2("input-hashes.csv");

    while (!file2.eof()) {
        file2 >> h_str;
    
    #pragma omp parallel for  /* search parallel in all rainbow tables */  
        for (table_id = 0 ; table_id < num_of_tables ; table_id++) {
            map<string, string> chain_dict;   
            char table_name[15];
            sprintf(table_name, "rainbow_%zu.csv", table_id);

            /* create hash-map for quick checking and retrieving the first link of the chain */
            ifstream file(table_name);
            string first_pass, last_h;
            while (!file.eof()) {
                file >> first_pass;
                file >> last_h;
                // cout << "PASS:-" << first_pass << "- H:-" << last_h << "-\n";
                chain_dict.insert(make_pair(last_h, first_pass));
            }

            char* passwd = search_RT(h_str, chain_dict, chain_len);
            if (passwd == NULL) {
                cout << "Thread " << table_id << " didnt manage to find the password" << "\n";
            } else {
                cout << "Thread " << table_id << " found: " << passwd << "\n";
            }
        }
    }

    return EXIT_SUCCESS;
}
