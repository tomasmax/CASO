// SqliteMap.h
// author: dipina@eside.deusto.es
#ifndef __SQLITEMAP_H
#define __SQLITEMAP_H

extern "C" {
	#include <sqlite3.h>	
}

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <cstdlib>

using namespace std;

namespace PracticaCaso
{
	class SQLiteMap {
		private:
			sqlite3 *dbh;;
			string fileName;
			map<string, string> dns2IpPortMap;

			void close();
			void loadMappings(string mappinsFileName);
		public:
			SQLiteMap(string fn);
			SQLiteMap(const SQLiteMap&);
			~SQLiteMap();
			
			string get(string key);
			void set(string key, string value);
			map<string, string> getMap();
			friend ostream & operator << (ostream &os, SQLiteMap &t);
	};
};
#endif
