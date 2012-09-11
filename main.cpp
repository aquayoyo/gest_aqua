#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <iostream>

#include <sqlite3.h>
using namespace std;
	
	
static int callback(void *NotUsed, int argc, char **argv, char **azColName){
  int i;
  for(i=0; i<argc; i++){
    printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
  }
  printf("callback\n");
  return 0;
}
 
int main(int argc, const char **argv) {		
	sqlite3 *dBase=NULL;
	int idBase=-1;

	idBase=sqlite3_open ("param/AquaGest.sl3",&dBase);
	if (idBase==SQLITE_OK) {	
		
		
		if( sqlite3_exec(dBase, "select * from Systeme", callback, NULL, NULL)!=SQLITE_OK ){
			cout << sqlite3_errmsg(dBase);
		}
		sqlite3_close (dBase);
	}else 
		cout << sqlite3_errmsg(dBase);
	while (1) {
		usleep (100);
	}
    cout << "Done." << endl;
    return EXIT_SUCCESS;
}