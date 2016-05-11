#include <iostream>
#include <fstream>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

extern "C" {
    #include "blake.h"
    #include "createRT.h"
}

using namespace std;

char* search_RT(string h, map<string, string> chain_dict, size_t chain_len) {
	int i = 0;
	while (i++ < chain_len) {

		// const uint8_t* h = reinterpret_cast<const uint8_t*>(h_str.c_str());
		map<string, string>::iterator it = chain_dict.find(h);

		if (it == chain_dict.end()) { /* Not found */ 
		
		} else { /* i->first will contain h, i->second will contain pass */ 

		}
		

	}
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
		while (!file.eof()) {
			file >> first_pass;
			file >> last_h;
	        cout << "PASS: " << first_pass << " H: " << last_h << "\n";
	        chain_dict.insert(make_pair(last_h, first_pass));
		}

		string h_str;
		cout << "Enter hash to search in the rainbow-tables: ";
		getline(cin, h_str);
		char* pass = search_RT(h_str, chain_dict, chain_len);

	}

    return EXIT_SUCCESS;
}
