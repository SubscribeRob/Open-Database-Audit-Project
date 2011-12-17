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

#ifndef MYSQLCONNECTION_H_
#define MYSQLCONNECTION_H_

#include "DatabaseConnection.h"
#include <boost/regex.hpp>

class MySQLConnection: public DatabaseConnection {
private:
	static log4cxx::LoggerPtr logger;
public:
	MySQLConnection();
	virtual ~MySQLConnection();

protected:
	bool parseRequest(int data_offset);
	bool parseReponse();
};

#endif /* MYSQLCONNECTION_H_ */
