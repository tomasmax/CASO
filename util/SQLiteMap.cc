// SQLiteMap.cc
// author: dipina@eside.deusto.es
// compile: g++ SQLiteMap.cc -lsqlite3 -o SQLiteMap
#include "SQLiteMap.h"

namespace PracticaCaso {
	SQLiteMap::SQLiteMap(string fn): fileName(fn), dbh(0) {
		// Process the contents of the mapping file
		this->loadMappings(fn);
	}

	SQLiteMap::SQLiteMap(const SQLiteMap& rhs) {
		fileName = rhs.fileName;
		dbh = rhs.dbh;
		this->loadMappings(fileName);
	}

	SQLiteMap::~SQLiteMap() {
		cout << "SQLiteMap: destructor called " << endl;
		this->close();
	}

	void SQLiteMap::loadMappings(string mappingsDBFileName) {
		// Loads the mappings stored at SQLite DB into the map loadMappings
		// In the case that the DB does not exist, create it, its structure is given by file KeyValueDB.sql
		// If a select * from KeyValuePair executed through a sqlite3_get_table does not return SQLITE_OK, it means that the table does not exist, and needs being created
		// If there are unexpected error exit the program with exit(1)
		
		char **result;
		int nrow;
		int ncol;
		char *errorMsg;
		
		cout << "Opening KeyValuePair database ..." << endl;
		if (sqlite3_open(mappingsDBFileName, &this->dbh) != SQLITE_OK) {
				cout << "KeyValuePair can not be opened ..." << endl;
		}
		
		if (sqlite3_get_table(this->dbh, "select * from KeyValuePair", &result, &nrow, &ncol, &errorMsg) != SQLITE_OK) {
			cerr << errorMsg << endl;
			sqlite3_free(errorMsg);
			if (sqlite3_get_table(this->dbh, "create table KeyValuePair(key_element BLOB NOT NULL PRIMARY KEY, value_element BLOB)", &result, &nrow, &ncol, &errorMsg) != SQLITE_OK) {
				cerr << errorMsg << endl;
				sqlite3_free(errorMsg);
				sqlite3_close(this->dbh);
			} else {
				cout << "Table KayValuePair created" << endl;
			}
			
			//Cargamos al mapa
			cout << "Inserting values to dns2IpPortMap from DB..." << endl;
			for (int i=0; i<=nrow; i++) {
				this->dns2IpPortMap[string(result[i*ncol])] = string(result[i*ncol+1]);
			}
			sqlite3_free_table(result);
			
		} 
		
	
	}

	
	map<string, string> SQLiteMap::getMap() {
		return this->dns2IpPortMap;
	}


	string SQLiteMap::get(string key) {
		map<string, string>::iterator i;
		i = this->dns2IpPortMap.find(key);
		
		if (i != this->dns2IpPortMap.end())
			return i->second;
		else
			return "ERROR";
		
	}

	void SQLiteMap::set(string mapKey, string mapValue) {
		// Undertake the update of the STL map and the database. Bear in mind that it there is not an entry with a passed key an INSERT will have 
		//to be executed, 
		//if there was already such an entry an UPDATE will take place
		
		string sql;
		char **result;
		int nrow;
		int ncol;
		char *errorMsg;
		
		map<string, string>::iterator i;
		i = this->dns2IpPortMap.find(mapKey);
		
		if (i != this->dns2IpPortMap.end()) {
			// It doesnt exist in the map, we have to insert into DB
			sql =  "insert into KeyValuePair( key_element, value_element) values ('" + mapKey + "', '" + mapValue + "')";
		} else {
			// It already exists in the map, we have to update the DB
			sql =  "update KeyValuePair set value_element='" + mapValue + "' where key_element='" + mapKey + "'";
		}
		
		if (sqlite3_get_table(this->dbh, sql.c_str(), &result, &nrow, &ncol, &errorMsg) != SQLITE_OK) {
				cerr << errorMsg << endl;
				sqlite3_free(errorMsg);
				sqlite3_close(this->dbh);
		} else {
				cout << "Table KayValuePair inserted/updated" << endl;
				sqlite3_free_table(result);
		}
		
		// We update the map
		this->dns2IpPortMap[mapKey] = mapValue;
		
		
	}


	void SQLiteMap::close() {
		// Close the database properly
		sqlite3_close(this->dbh);
		cout << "Database closed properly" << endl;
	}

	ostream & operator << (ostream & os, SQLiteMap &t) {
		os << "DB file name: " << t.fileName << endl;
		os << "DNS mappings:" << endl;
		typedef map<string, string>::const_iterator CI;
		for (CI p = t.dns2IpPortMap.begin(); p != t.dns2IpPortMap.end(); ++p) {
			os <<  p->first << " : " << p->second << endl;
		}
		return os;
	}
}


/*
// global variable
PracticaCaso::SQLiteMap * SQLiteMap_pointer;

// function called when CTRL-C is pressed
void ctrl_c(int)
{
    printf("\nCTRL-C was pressed...\n");
	delete SQLiteMap_pointer;
}


void usage() {
	cout << "Usage: SQLiteMap <name-mappings-db-file>" << endl;
	exit(1);
}

int main(int argc, char** argv) {
	signal(SIGINT,ctrl_c);

	if (argc != 2) {
		usage();
	}

	PracticaCaso::SQLiteMap * SQLiteMap = new PracticaCaso::SQLiteMap((string)argv[1]);
	cout << "SQLiteMap instance: " << endl << SQLiteMap << endl;
	//SQLiteMap_pointer = &SQLiteMap;
	SQLiteMap_pointer = SQLiteMap;

	//SQLiteMap.set("saludo", "hola");
	//SQLiteMap.set("despedida", "adios");
	SQLiteMap->set("saludo", "hola");
	SQLiteMap->set("despedida", "adios");

	//cout << "SQLiteMap[\"saludo\"] = " << SQLiteMap.get("saludo") << endl;
	//cout << "SQLiteMap[\"despedida\"] = " << SQLiteMap.get("despedida") << endl;
	cout << "SQLiteMap[\"saludo\"] = " << SQLiteMap->get("saludo") << endl;
	cout << "SQLiteMap[\"despedida\"] = " << SQLiteMap->get("despedida") << endl;

	cout << "SQLiteMap instance after 2 sets: " << endl << *SQLiteMap << endl;

	//map<string, string> savedSQLiteMap = SQLiteMap.getMap();
	map<string, string> savedSQLiteMap = SQLiteMap->getMap();
	cout << "Recuperado el mapa" << endl;
	typedef map<string, string>::const_iterator CI;
	for (CI p = savedSQLiteMap.begin(); p != savedSQLiteMap.end(); ++p) {
		cout << "dns2IpPortMap[" << p->first << "] = " << p->second << endl;
	}
	
	delete SQLiteMap_pointer;
}
*/

