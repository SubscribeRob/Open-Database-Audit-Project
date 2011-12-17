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

#ifndef DATABASECONNECTION_H_
#define DATABASECONNECTION_H_


#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include "../../include/charsets.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pcap.h>
#include "log4cxx/logger.h"
#include "protocol/AuditEvent.h"
#include "../../CachedUserManager.h"
#include <vector>
#include <time.h>
#include <boost/regex.hpp>
using namespace log4cxx;
using namespace std;
using namespace boost;

extern AuditEventClient * client_store;

class DatabaseConnection {
private:
	static log4cxx::LoggerPtr logger;

protected:
	  const struct ip* 		ip_pkt;		//IP Packet
	  const struct tcphdr* 	tcp_pkt;	//TCP Packet
	  Event event;						//Holds information for the frame being inspected
	  unsigned char packet_data[128*1024];		//Holds a copy of the current packet
	  int  packet_data_length;
	  CachedUserManager userManager;
	  int packet_flag;	//TCP FLAG - Used for looking for SYN on new connections
	  struct timespec last_statement_time;
public:
	DatabaseConnection();
	virtual ~DatabaseConnection();

	bool parse(const u_char * packet,pcap_pkthdr * header,int data_offset);
	bool parse(  char *data, int len);
	bool parse_format_2(char * data, int len);

protected:
	virtual bool parseRequest(int data_offset) = 0;
	virtual bool parseReponse() = 0;
	boost::regex * predicate_regex;
	string remove_predicates(regex& re,const string & input);


	void parseHeader(const u_char * packet,pcap_pkthdr * header,int data_offset);
	std::string itoa(int value, int base);


};

#endif /* DATABASECONNECTION_H_ */
