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

#include "DB2Connection.h"
#include <string.h>
#include "../../include/charsets.h"
#include "../../EventQueue.h"
using namespace std;
LoggerPtr  DB2Connection::logger(Logger::getLogger("com.dbaudit.DB2Connection"));
extern EventQueue event_queue;


DB2Connection::DB2Connection() {
	LOG4CXX_DEBUG(logger,"DB2Connection()");

}

DB2Connection::~DB2Connection() {
	LOG4CXX_DEBUG(logger,"~DB2Connection()");
}

bool DB2Connection::parseRequest(int data_offset)
{
	//if(header->caplen < 80){
  	//	  continue;
  	  //}

	LOG4CXX_DEBUG(logger,"Entering DB2Connection::parseRequest()");

	if(packet_data_length < 10){
		LOG4CXX_DEBUG(logger,"Packet too small to look at");
		return true;
	}
	int packet_data_offset = 0; //Pointer to where we are in processing the packet

	int code_point = 0; // What type of information is being sent
	int previous_code_point = 0;
	bool first_loop = true; //As multiple pieces of information are in one packet is this the first run
	int frame_length = 0 ; // Length of the entire packet
	int offset_counter = 0;


	LOG4CXX_DEBUG(logger,L"Packet Data Length:" << packet_data_length);

	while(packet_data_offset < packet_data_length && packet_data_length > 0){

		int ddm_length = packet_data[packet_data_offset]<<8 | packet_data[packet_data_offset+1];
		int format =  packet_data[packet_data_offset+3];

		LOG4CXX_DEBUG(logger,L"DDM Length:" << ddm_length);
		LOG4CXX_DEBUG(logger,L"Packet data offset:"  << packet_data_offset);

    	if(first_loop){
    		frame_length = ddm_length;
    		first_loop = true;
    		offset_counter = 0;
    		code_point = packet_data[packet_data_offset+8]<<8 | packet_data[packet_data_offset+9];
    	}else{
    		previous_code_point = code_point;
    		code_point = packet_data[packet_data_offset+2]<<8 | packet_data[packet_data_offset+3];
    	}

    	LOG4CXX_DEBUG(logger,L"Code Point:" << std::hex << code_point);

        if(code_point == 0x2110){
        	packet_data[packet_data_offset+ddm_length-1] = 0;
        	EBCDIC_to_ASCII((unsigned char*)(&packet_data[0]+packet_data_offset+4),strlen((char*)(&packet_data[0]+packet_data_offset+4)));
        	userManager.regDBName(event.src_ip,itoa(event.src_port, 10),(char*)(&packet_data[0]+packet_data_offset+4));
        	LOG4CXX_INFO(logger,L"Database name:" << (char*)(&packet_data[0]+packet_data_offset+4) );
        }

        if(code_point == 0x115e){
        	packet_data[packet_data_offset+ddm_length-1] = 0;
        	EBCDIC_to_ASCII((unsigned char*)(&packet_data[0]+packet_data_offset+4),strlen((char*)(&packet_data[0]+packet_data_offset+4)));
        	userManager.regAppName(event.src_ip,itoa(event.src_port, 10),(char*)(&packet_data[0]+packet_data_offset+4));
        	LOG4CXX_INFO(logger,L"Application name:" << (char*)(&packet_data[0]+packet_data_offset+4) );
        }

        if(code_point == 0x11a0){
        	packet_data[packet_data_offset+ddm_length] = 0;
        	EBCDIC_to_ASCII((unsigned char*)(&packet_data[0]+packet_data_offset+4),strlen((char*)(&packet_data[0]+packet_data_offset+4)));
        	userManager.regUsername(event.src_ip,itoa(event.src_port, 10),(char*)(&packet_data[0]+packet_data_offset+4));
        	LOG4CXX_INFO(logger,L"User ID:" << (char*)(&packet_data[0]+packet_data_offset+4) );
        }


        if(code_point == 0x0000){   // Identifier for SQLSTT (SQL Statement)
        	ddm_length = (packet_data[packet_data_offset-10]<<8 | packet_data[packet_data_offset-9])-10;
        	if(previous_code_point == 0x2414){
        		packet_data[packet_data_offset+ddm_length-1] = 0;
        		LOG4CXX_INFO(logger,L"Statement:" << (char*)(&packet_data[0]+packet_data_offset+5) );
    			userManager.regStatement(event.src_ip,itoa(event.src_port, 10),(char*)(&packet_data[0]+packet_data_offset+5));
    			userManager.loadEventAttributes(event.src_ip,itoa(event.src_port, 10),event);
    			event_queue.addMessage(event);
        	}
        	first_loop = false;

        	//printf("New ddm_length %d",ddm_length);
        }
        if(first_loop){
        	// If there are any more tracking issues look at format for correct restructuring
        	if(format == 0x53){
        		packet_data_offset+= ddm_length;
        		offset_counter+=ddm_length;
        	}else{
    			if((ddm_length > 13) ){
    				first_loop = false;
    				packet_data_offset+= 10;
    				offset_counter+=10;
    			}else{
    				if(ddm_length == 0){
    					packet_data_offset += (frame_length);
    					offset_counter += (frame_length);
    				}else{
    					packet_data_offset+= ddm_length;
    					offset_counter+=ddm_length;
    				}
    			}
        	}
        }else{
    		if(ddm_length == 0){
    			packet_data_offset = (frame_length);
    			offset_counter = (frame_length);
    		}else{
    			packet_data_offset += ddm_length;
    			offset_counter += ddm_length;
    		}
        	if(offset_counter == frame_length){
        		first_loop=true;
        		offset_counter =0;
        	}

        }
	}

	LOG4CXX_DEBUG(logger,"Exiting DB2Connection::parseRequest()");
	return true;
}

bool DB2Connection::parseReponse()
{return false;
}
