/*
 * DB2Connection.h
 *
 *  Created on: Nov 29, 2011
 *      Author: root
 */

#ifndef DB2CONNECTION_H_
#define DB2CONNECTION_H_

#include "DatabaseConnection.h"

class DB2Connection: public DatabaseConnection {
private:
	static log4cxx::LoggerPtr logger;
public:
	DB2Connection();
	virtual ~DB2Connection();
	bool parseRequest(int data_offset);
	bool parseReponse();
};

#endif /* DB2CONNECTION_H_ */
