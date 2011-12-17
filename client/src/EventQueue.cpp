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

#include "EventQueue.h"
#include <string>
#include <boost/thread.hpp>
#include <boost/date_time.hpp>
#include "protocol/AuditEvent.h"
using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

extern int server_id;
extern string token;
extern AuditEventClient * client_store;

EventQueue::~EventQueue() {
	// TODO Auto-generated destructor stub
}

void EventQueue::addMessage(Event event)
{
	boost::mutex::scoped_lock lock(guard);
	if(message == NULL){
		message = getNewMessage();
	}
	if(message->events.size() < 100000){
		message->events.push_back(event);
	}
}



void EventQueue::sendMessage()
{
	Message * previous_message = NULL;
	bool delete_old_message = false;

	{
		boost::mutex::scoped_lock lock(guard);
		if(message != NULL && !message->events.empty()){
				previous_message = message;
				message = getNewMessage();
				delete_old_message = true;
		}
	}
		if((last_heartbeat - time(0)) > 60*5 ){
			client_store->heartbeat(server_id,token);
		}

		if(delete_old_message){
			client_store->store(*previous_message);
			delete previous_message;
		}


}

Message *EventQueue::getNewMessage()
{
	Message * tmp = new Message();
	tmp->server_id = server_id;
	tmp->token = token;
	return tmp;
}


EventQueue::EventQueue() {
	message = NULL;
	last_heartbeat = 0;
}




