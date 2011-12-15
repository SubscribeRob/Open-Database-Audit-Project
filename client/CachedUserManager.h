/* 
 *   This file is part of the Open Database Audit Project (ODAP).
 *
 *   ODAP is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Foobar is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 *
 *   The code was developed by Rob Williams
 */

#ifndef CACHEDUSERMANAGER_H_
#define CACHEDUSERMANAGER_H_

#include <map>
#include <string>
#include "CachedUserInfo.h"
#include "protocol/auditevent_types.h"

#define MAX_CACHED_CONNECTIONS 65000

using namespace std;


class CachedUserManager {
private:
	map<string,CachedUserInfo> connection_map;

	string getKey(string ip_address,string port);
	void cleanConnectionMap();

public:
	void loadEventAttributes(string ip_address,string port,Event & event);
	void fin(string ip_address,string port);
	void tcp_flag(string ip_address,string port,int tcp_flag);
	int  getSeq(string ip_address, string port);
	void regDBName(string ip_address, string port, string dbname);
	void regAppName(string ip_address, string port, string appname);
	void regUsername(string ip_address, string port, string username);
	void regRowCount(string ip_address, string port, int row_count);
	void regStatement(string ip_address, string port, string statement);
	void regEventTimestamp(string ip_address, string port,event_timestamp evt_ts);

	CachedUserInfo getUserInfo(string ip_address, string port);

	CachedUserManager();
	virtual ~CachedUserManager();
};

#endif /* CACHEDUSERMANAGER_H_ */
