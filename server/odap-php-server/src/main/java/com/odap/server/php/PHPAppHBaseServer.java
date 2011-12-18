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
package com.odap.server.php;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.hbase.HBaseConfiguration;
import org.apache.hadoop.hbase.client.HTable;
import org.apache.log4j.Logger;
import org.apache.thrift.server.TServer;
import org.apache.thrift.server.TSimpleServer;
import org.apache.thrift.server.TThreadPoolServer;
import org.apache.thrift.transport.TServerSocket;
import org.apache.thrift.transport.TServerTransport;

import com.odap.common.thrift.*;


public class PHPAppHBaseServer {
	 
	public static PHPAppHBaseHandler handler;
	public static DisplayEvents.Processor processor; 
	public static HTable table;
	static Logger logger = Logger.getLogger(PHPAppHBaseServer.class);
	
	public static void main(String [] args) {
		try{
			handler = new PHPAppHBaseHandler(); 
			processor = new DisplayEvents.Processor(handler);
			Configuration conf = HBaseConfiguration.create(); 
			// conf.set("hbase.zookeeper.quorum", "server1,server2,server3");
	
			table = new HTable(conf, "sql_audit"); 
			Runnable simple = new Runnable() {
	          public void run() {
	            simple(processor);
	          }
	      };
	      
	      new Thread(simple).start();
		}catch(Exception e){
			e.printStackTrace();
		}
	}
	  public static void simple(DisplayEvents.Processor processor) {
		    try {
		      TServerTransport serverTransport = new TServerSocket(9091);
		      TServer server = new TThreadPoolServer(new TThreadPoolServer.Args(serverTransport).processor(processor));

		      logger.info("Thrift server started");
		      
		      server.serve();
		    } catch (Exception e) {
		      e.printStackTrace();
		    }
		  }
}
