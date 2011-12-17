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

#include "MySQLConnection.h"
#include <string.h>
#include "../../EventQueue.h"
extern EventQueue event_queue;

LoggerPtr  MySQLConnection::logger(Logger::getLogger("com.dbaudit.MySQLConnection"));
extern bool strip_predicates;
MySQLConnection::MySQLConnection() {
	LOG4CXX_DEBUG(logger,"MySQLConnection()");
	predicate_regex = new boost::regex("(\"([^\"\\\\]|\\\\.|\"\".)*\")|('([^'\\\\]|\\\\.|\\s|''.)*')|((\\s|[\\(\\)\\/\\+\\-\\*\\^\\}\\{=,]+)[0-9]+)");
}

MySQLConnection::~MySQLConnection() {
	LOG4CXX_DEBUG(logger,"~MySQLConnection()");
	delete predicate_regex;
}

bool MySQLConnection::parseRequest(int data_offset){
	LOG4CXX_DEBUG(logger,"Entering parseRequest()");

	int length = 0;
	char* packet = (char*)&packet_data[0+data_offset];
	char type = packet[4];
	packet[packet_data_length] = 0;

	LOG4CXX_DEBUG(logger,L"Packet packet length:"  << packet_data_length << " SEQ: " << userManager.getSeq(event.src_ip,itoa(event.src_port, 10)));
	LOG4CXX_DEBUG(logger,L"Packet type:" << hex<< (short)type);

	if( type == (char)0x85) { //CONNECT
			LOG4CXX_INFO(logger,L"Username:"  << (const char*)packet+36);
			userManager.regUsername(event.src_ip,itoa(event.src_port, 10),(char*)(packet+36));
			length = packet[strlen((char*)(packet+36))+36+1];
			LOG4CXX_INFO(logger,L"DBName:"  << (const char *)packet+strlen((char*)(packet+36))+36+length+2);
			userManager.regDBName(event.src_ip,itoa(event.src_port, 10),(const char *)packet+strlen((char*)(packet+36))+36+length+2);
	}
	if(type == (char)0x3  || type == (char)0x5 || type == (char)0x6 || type == (char)0x7 || type == (char)0x8 || type == (char)0x9 || type == (char)0xc || type == (char)0x16){
		LOG4CXX_INFO(logger,L"Query Type:"  << hex << type);
		LOG4CXX_INFO(logger,L"SQL Query:"  << (const char *)packet+5);
		 string statement;
		 if(strip_predicates){
			 statement =remove_predicates(*predicate_regex,string((char*)(packet+5)));
		 }else{
			 statement = (char*)(packet+5);
		 }
		 userManager.regStatement(event.src_ip,itoa(event.src_port, 10),statement);
		 userManager.loadEventAttributes(event.src_ip,itoa(event.src_port, 10),event);
		 event_queue.addMessage(event);
	}
	if(type == (char)0x2){ //SELECT DB
			LOG4CXX_INFO(logger,L"Select database:"  << (const char *)packet+5);
			userManager.regDBName(event.src_ip,itoa(event.src_port, 10),(const char *)packet+5);
	}
	if(type == (char)0x11){ //CHANGE USER
			LOG4CXX_INFO(logger,L"Change User:"  << (const char *)packet+5);
			userManager.regUsername(event.src_ip,itoa(event.src_port, 10),(char*)(packet+5));
	}


	LOG4CXX_DEBUG(logger,"Exiting parseRequest()");
	return true;
}

bool MySQLConnection::parseReponse(){
	return true;
}

