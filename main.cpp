#include <stdlib.h>
#include <iostream>
#include <sqlite3ext.h>

using namespace std;
SQLITE_EXTENSION_INIT1 
	
int main(int argc, const char **argv) {		
	sqlite3 *dBase;
	int idBase=-1;
	sqlite3_api_routines toto;
	SQLITE_EXTENSION_INIT2 (&toto);
	
	if (sqlite3_api) {
		cout << "1." << endl;
		idBase=sqlite3_open ("/home/lionel/olinuxino/src/gest_aqua/param/AquaGest.sl3",&dBase);
		cout << "2." << endl;
		/*if (sqlite3_api) {
			delete sqlite3_api;
			sqlite3_api=NULL;
		}*/
	}else
		cout << "Fail." << endl;
	
    cout << "Done." << endl;
    return EXIT_SUCCESS;
}