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
#include <boost/thread.hpp>
#include <boost/date_time.hpp>
#include "log4cxx/logger.h"
#include "log4cxx/basicconfigurator.h"
#include "log4cxx/helpers/exception.h"

using namespace log4cxx;
using namespace log4cxx::helpers;
EventQueue event_queue;
LoggerPtr  thread_logger(Logger::getLogger("com.opendbaudit.threads"));


void sendMessageThread()
{
	LOG4CXX_DEBUG(thread_logger,"In sendMessageThread");
	while(true){
		boost::posix_time::seconds workTime(5);
		LOG4CXX_DEBUG(thread_logger,"Wakeup send message");
		event_queue.sendMessage();
		boost::this_thread::sleep(workTime);
	}
}
