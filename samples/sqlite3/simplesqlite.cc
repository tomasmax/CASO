// Compile: g++ simplesqlite.cc -lsqlite3 -o simplesqlite
// Run: ./simplesqlite or simplesqlite.exe

#include <fstream>
#include <iostream>
#include <string>

extern "C" {
	#include <sqlite3.h>	
	#include <stdio.h>
}
using namespace std;

int main(int argc, char **argv){
	sqlite3 *db;

	cout << "Opening DnsMap database ..." << endl;
	if (sqlite3_open(argv[1], &db) != SQLITE_OK) {
		cerr << "Can't open database: " << sqlite3_errmsg(db) << endl;
		sqlite3_close(db);
		exit(1);
	}

	char **result;
	int nrow;
	int ncol;
	char *errorMsg;
	cout << "Checking if DnsMap table already exists ..." << endl;
	if (sqlite3_get_table(db, "select * from DnsMap", &result, &nrow, &ncol, &errorMsg) != SQLITE_OK) {
	  cerr << errorMsg << endl;
	  sqlite3_free(errorMsg);
	  if (sqlite3_get_table(db, "create table DnsMap(hostName varchar(200) NOT NULL PRIMARY KEY, ipAddress varchar(200))", &result, &nrow, &ncol, &errorMsg) != SQLITE_OK) {
		  cerr << errorMsg << endl;
		  sqlite3_free(errorMsg);
		  sqlite3_close(db);
		  exit(1);
	  } else {
		  cout << "Table DnsMap created" << endl;
		  sqlite3_free_table(result);
	  }
	} 

	cout << "Checking if there is at least one row in DnsMap ..." << endl;
	if (nrow == 0) {
		if (sqlite3_get_table(db, "insert into DnsMap values('deusto.es', 'localhost:1234')", &result, &nrow, &ncol, &errorMsg) != SQLITE_OK) {
			cerr << errorMsg << endl;
			sqlite3_free(errorMsg);
			sqlite3_close(db);
			exit(1);
		} else {
			cout << "Row created: " << nrow << endl;
			sqlite3_free_table(result);
		}
	}

	cout << "Listing contents of DnsMap ..." << endl;
	if (sqlite3_get_table(db, "select * from DnsMap", &result, &nrow, &ncol, &errorMsg) != SQLITE_OK) {
		cerr << errorMsg << endl;
		sqlite3_free(errorMsg);
	} else {
		// sqlite3 returns one extra row with the column headers
		for (int i=0; i<=nrow; i++) {
			cout << string(result[i*ncol]) + "--" + string(result[i*ncol+1]) << endl;
		}
		sqlite3_free_table(result);
	}

	cout << "Closing DnsMap database ..." << endl;
	sqlite3_close(db);
	return 0;
}
