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

#include "CachedUserManager.h"

CachedUserManager::CachedUserManager() {
	// TODO Auto-generated constructor stub

}

void CachedUserManager::loadEventAttributes(string ip_address,string port, Event & event)
{
	map<string,CachedUserInfo>::iterator it;
	it = connection_map.find(getKey(ip_address,port));

	if(it != connection_map.end()){
		event.__set_dbname(it->second.getDBName());
		event.__set_appname(it->second.getAppName());
		event.__set_statement(it->second.getStatement());
		event.__set_username(it->second.getUsername());
		event.__set_timestamp(it->second.getEventTimestamp().event_in_s);
	}
}

void CachedUserManager::fin(string  ip_address, string  port)
{
	connection_map.erase(getKey(ip_address,port));
}

void CachedUserManager::regDBName(string  ip_address, string  port, string dbname)
{
	map<string,CachedUserInfo>::iterator it;
	it = connection_map.find(getKey(ip_address,port));

	if(it != connection_map.end()){
		(it->second).setDBName(dbname);
	}else{

		CachedUserInfo userInfo;
		userInfo.setDBName(dbname);
		connection_map.insert( pair<string, CachedUserInfo>(getKey(ip_address,port),userInfo));
	}
}

void CachedUserManager::regAppName(string  ip_address, string  port, string appname)
{
	map<string,CachedUserInfo>::iterator it;
	it = connection_map.find(getKey(ip_address,port));

	if(it != connection_map.end()){
		(it->second).setAppName(appname);
	}else{

		CachedUserInfo userInfo;
		userInfo.setAppName(appname);
		connection_map.insert( pair<string, CachedUserInfo>(getKey(ip_address,port),userInfo));
	}
}

string CachedUserManager::getKey(string  ip_address, string  port)
{
	return ip_address + ":" + port;

}

void CachedUserManager::cleanConnectionMap()
{
	if(connection_map.size() > MAX_CACHED_CONNECTIONS){
		map<string,CachedUserInfo>::iterator it = connection_map.begin();

		while(it != connection_map.end()){
			if(it->second.isExpired()){
				connection_map.erase(it);
			}
		}

		if(connection_map.size() > MAX_CACHED_CONNECTIONS){
			exit(322);
		}
	}

}

void CachedUserManager::regUsername(string ip_address, string port, string username)
{
	map<string,CachedUserInfo>::iterator it;
	it = connection_map.find(getKey(ip_address,port));

	if(it != connection_map.end()){
		(it->second).setUsername(username);
	}else{

		CachedUserInfo userInfo;
		userInfo.setUsername(username);
		connection_map.insert( pair<string, CachedUserInfo>(getKey(ip_address,port),userInfo));
	}
}

void CachedUserManager::regRowCount(string ip_address, string port, int row_count)
{
	map<string,CachedUserInfo>::iterator it;
	it = connection_map.find(getKey(ip_address,port));

	if(it != connection_map.end()){
		(it->second).setRowCount(row_count);
	}else{

		CachedUserInfo userInfo;
		userInfo.setRowCount(row_count);
		connection_map.insert( pair<string, CachedUserInfo>(getKey(ip_address,port),userInfo));
	}
}

void CachedUserManager::regStatement(string ip_address, string port,  string statement)
{
	map<string,CachedUserInfo>::iterator it;
	it = connection_map.find(getKey(ip_address,port));

	if(it != connection_map.end()){
		(it->second).setStatement(statement);
	}else{

		CachedUserInfo userInfo;
		userInfo.setStatement(statement);
		connection_map.insert( pair<string, CachedUserInfo>(getKey(ip_address,port),userInfo));
	}
}

void CachedUserManager::tcp_flag(string ip_address, string port,int tcp_flag)
{
	map<string,CachedUserInfo>::iterator it;
	it = connection_map.find(getKey(ip_address,port));

	if(it != connection_map.end()){
		(it->second).setTcpFlag(tcp_flag);
	}else{

		CachedUserInfo userInfo;
		userInfo.setTcpFlag(tcp_flag);
		connection_map.insert( pair<string, CachedUserInfo>(getKey(ip_address,port),userInfo));
	}
}

int CachedUserManager::getSeq(string ip_address, string port)
{
	map<string,CachedUserInfo>::iterator it;
		it = connection_map.find(getKey(ip_address,port));

		if(it != connection_map.end()){
			return (it->second).getSequence();
		}
}

CachedUserManager::~CachedUserManager() {
	// TODO Auto-generated destructor stub
}

void CachedUserManager::regEventTimestamp(string ip_address, string port,event_timestamp evt_ts){
	map<string,CachedUserInfo>::iterator it;
	it = connection_map.find(getKey(ip_address,port));

	if(it != connection_map.end()){
		(it->second).setEventTimestamp(evt_ts);
	}else{

		CachedUserInfo userInfo;
		userInfo.setEventTimestamp(evt_ts);
		connection_map.insert( pair<string, CachedUserInfo>(getKey(ip_address,port),userInfo));
	}
}

CachedUserInfo CachedUserManager::getUserInfo(string ip_address, string port){
	map<string,CachedUserInfo>::iterator it;
	it = connection_map.find(getKey(ip_address,port));

	if(it != connection_map.end()){
		return it->second;
	}else{
		CachedUserInfo userInfo;
		connection_map.insert( pair<string, CachedUserInfo>(getKey(ip_address,port),userInfo));
		return userInfo;
	}

}
