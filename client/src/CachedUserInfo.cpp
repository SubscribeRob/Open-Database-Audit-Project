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

#include "CachedUserInfo.h"
#include <time.h>

CachedUserInfo::CachedUserInfo() {
	last_accessed_timestamp = 0;
    row_count = 0;
    tcp_flag = 0;
    sequence = 900000000;
    event_ts.event_in_s = 0;
    event_ts.event_in_ms = 0;
}

string CachedUserInfo::getDBName()
{
	last_accessed_timestamp = time (NULL);
	return dbname;
}

string CachedUserInfo::getAppName()
{
	last_accessed_timestamp = time (NULL);
	return appname;
}

void CachedUserInfo::setDBName(string dbname)
{
	last_accessed_timestamp = time (NULL);
	this->dbname = dbname;
}

void CachedUserInfo::setAppName(string appname)
{
	last_accessed_timestamp = time (NULL);
	this->appname = appname;
}

bool CachedUserInfo::isExpired()
{
	if((time (NULL) -last_accessed_timestamp) > 86400){
		return true;
	}
	return false;
}

string CachedUserInfo::getUsername()
{
	last_accessed_timestamp = time (NULL);
    return username;
}

int CachedUserInfo::getRowCount()
{
	last_accessed_timestamp = time (NULL);
	return row_count;
}

void CachedUserInfo::setRowCount(int row_count)
{
	last_accessed_timestamp = time (NULL);
	this->row_count = row_count;
}

void CachedUserInfo::setStatement(string statement)
{
	last_accessed_timestamp = time (NULL);
	this->statement = statement;
}

string CachedUserInfo::getStatement()
{
	last_accessed_timestamp = time (NULL);
	return statement;
}

void CachedUserInfo::setUsername(string username)
{
	last_accessed_timestamp = time (NULL);
    this->username = username;
}

CachedUserInfo::~CachedUserInfo() {
	// TODO Auto-generated destructor stub
}

int CachedUserInfo::getTcpFlag()
{
	return tcp_flag;
}



void CachedUserInfo::setTcpFlag(int tcp_flag)
{
	last_accessed_timestamp = time (NULL);
	this->tcp_flag = tcp_flag;
	if(tcp_flag == 2){
		setSequence(0);
	}else{
		this->setSequence(this->getSequence()+1);
	}
}

int CachedUserInfo::getSequence()
{
	last_accessed_timestamp = time (NULL);
	return sequence;
}



void CachedUserInfo::setSequence(int sequence)
{
	last_accessed_timestamp = time (NULL);
	if(sequence > 1000000000){
		this->sequence = 10;
	}
	this->sequence = sequence;
}

event_timestamp CachedUserInfo::getEventTimestamp(){
	last_accessed_timestamp = time (NULL);
	return event_ts;
}

void CachedUserInfo::setEventTimestamp(event_timestamp event_ts){
	last_accessed_timestamp = time (NULL);
	this->event_ts.event_in_s = event_ts.event_in_s;
	this->event_ts.event_in_ms = event_ts.event_in_ms;
}
