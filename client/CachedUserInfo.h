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

#ifndef CACHEDUSERINFO_H_
#define CACHEDUSERINFO_H_


#include<string>
using namespace std;


struct event_timestamp{
    long event_in_s;
    long event_in_ms;
};

class CachedUserInfo {
private:
	int last_accessed_timestamp;
	string dbname;
	string appname;
    string username;
    string statement;
    int row_count;
    int tcp_flag;
    int sequence;

    event_timestamp event_ts;

public:
    CachedUserInfo();
    string getDBName();
    string getAppName();
    string getStatement();
    int getRowCount();
    void setDBName(string dbname);
    void setAppName(string appname);
    void setStatement(string statement);
    void setRowCount(int row_count);
    bool isExpired();
    virtual ~CachedUserInfo();
    string getUsername() ;
    void setUsername(string username);

    int getTcpFlag();
    void setTcpFlag(int tcp_flag);

    int getSequence();
    void setSequence(int sequence);

    event_timestamp getEventTimestamp();
    void setEventTimestamp(event_timestamp event_ts);
};

#endif /* CACHEDUSERINFO_H_ */
