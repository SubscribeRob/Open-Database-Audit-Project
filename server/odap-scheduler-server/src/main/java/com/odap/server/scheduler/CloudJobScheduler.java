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

package com.odap.server.scheduler;

import java.net.ServerSocket;
import java.net.Socket;
import java.sql.*;

import org.apache.log4j.BasicConfigurator;
import org.apache.log4j.Logger;

public class CloudJobScheduler {

	static Logger logger = Logger.getLogger(CloudJobScheduler.class);
	static JobManager manager;
	public static void main(String[] args) {
		
    	
        logger.info("Enter CloudJobScheduler:main()");
        		
		manager = new JobManager();
		manager.refreshJobs();
		
		Runnable r = new Runnable() {
			  public void run() {
				  try {
					  ServerSocket serverSocket = new ServerSocket(4444);
					  Socket clientSocket = null;
					  while((clientSocket = serverSocket.accept()).isConnected()){
						  logger.info("Refresh requested of TCP/IP");
						  manager.refreshJobs();
						  clientSocket.close();
					  }
					} catch (Exception e) {
					    logger.error("Error with refresh thread socket server",e);
					}
			  }
			};
		r.run();
	    try {
	        while (true) {
	            logger.info("Refreshing jobs");
	        	manager.refreshJobs();
	            Thread.sleep(1 * 5000*60);
	        }
	    } catch (InterruptedException e) {
	        e.printStackTrace();
	    }
			
		logger.info("Exiting CloudJobScheduler:main()");
	}

}
