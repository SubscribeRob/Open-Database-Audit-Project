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

#ifndef EVENTQUEUE_H_
#define EVENTQUEUE_H_
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include "protocol/auditevent_types.h"

class EventQueue {
private:
	boost::mutex guard;
	Message * message;
	Message * getNewMessage();

	int last_heartbeat;
public:

	void addMessage(Event event);
	void sendMessage();
	EventQueue();
	virtual ~EventQueue();
};

#endif /* EVENTQUEUE_H_ */
