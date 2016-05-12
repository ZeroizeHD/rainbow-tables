#include <iostream>
#include <fstream>
#include <map>
#include <stdio.h>
#include <stdlib.h>
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

char* search_chain(string first_pass, size_t chain_len) {

};

char* search_RT(string h, map<string, string> chain_dict, size_t chain_len) {
	// int i = 0;
	// const uint8_t* temp = reinterpret_cast<const uint8_t*>(h.c_str());
	// uint8_t* hash = (uint8_t *) malloc(32 * sizeof(uint8_t));
	// memcpy(hash, temp, 32);

	// while (i++ < chain_len) {
		string first_pass = chain_dict[h];
		if (first_pass != "") { /* if found*/  
			return search_chain(first_pass, chain_len);
		} else { 
			const uint8_t *hash2 = reinterpret_cast<const uint8_t*>(&h[0]);
			uint8_t *hash = (uint8_t*) malloc(64 * sizeof(uint8_t));
			memcpy(hash, hash2, 64);
        	char *pass = reduction(hash);
        	cout << "--" << hash << "--" << endl;
        	cout << "--" << pass << "--" << endl;

        	blake256_hash(hash, (uint8_t*)pass, 6);


		}
	// }
	return NULL;
}

int main(int argc, char **argv) {
	size_t rainbow_size = (argc == 2) ? atoi(argv[1]) : RAINBOW_SIZE; 
    rainbow_size = (rainbow_size > 0) ? rainbow_size : RAINBOW_SIZE;
    size_t table_id;
    size_t num_of_tables = NUM_OF_TABLES;
    size_t chain_len = CHAIN_LEN;

	/* for all rainbow tables */    
	for (table_id = 0 ; table_id < num_of_tables ; table_id++) {
		map<string, string> chain_dict;   
	    char table_name[15];
    	sprintf(table_name, "rainbow_%zu.csv", table_id);

		/* create hash-map for quick checking and retrieving the first link of the chain */
		ifstream file(table_name);
		string first_pass, last_h;
		// string first_pass;
		// uint8_t last_h[33;
		while (!file.eof()) {
			file >> first_pass;
			file >> last_h;
	        cout << "PASS: " << first_pass << " H: " << last_h << "\n";
	        chain_dict.insert(make_pair(last_h, first_pass));
		}

		string h_str;
		// cout << "Enter hash to search in the rainbow-tables: ";
		// getline(cin, h_str);
		// uint8_t h_str[33];
		ifstream file2("example.csv");
		file2 >> h_str;
		// cout << h_str << "-->" << chain_dict[h_str] << endl;
		char* pass = search_RT(h_str, chain_dict, chain_len);

	}

    return EXIT_SUCCESS;
}
