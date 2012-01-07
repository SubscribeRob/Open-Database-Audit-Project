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
package com.odap.server.audit;

import com.jolbox.bonecp.BoneCP;
import com.jolbox.bonecp.BoneCPConfig;
import com.odap.common.thrift.AuditEvent;
import com.odap.common.thrift.Config;
import com.odap.common.util.LogUtil;

import java.net.InetAddress;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;

import org.apache.commons.dbutils.DbUtils;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.hbase.HBaseConfiguration;
import org.apache.hadoop.hbase.client.HTable;
import org.apache.log4j.Logger;
import org.apache.thrift.server.TServer;
import org.apache.thrift.server.TServer.Args;
import org.apache.thrift.server.TSimpleServer;
import org.apache.thrift.server.TThreadPoolServer;
import org.apache.thrift.transport.TSSLTransportFactory;
import org.apache.thrift.transport.TServerSocket;
import org.apache.thrift.transport.TServerTransport;
import org.apache.thrift.transport.TSSLTransportFactory.TSSLTransportParameters; 



public class AuditServer {

	public static AuditHandler handler;
	public static AuditEvent.Processor processor; 
	public static ConfigHandler config_handler; 
	public static Config.Processor<ConfigHandler> config_processor; 
	public static HTable table;
	public static byte audit_server_id = 0x01;
	public static BoneCP connectionPool = null;
	public static String[] args_cpy;
	static Logger logger = LogUtil.getInstance();
	private static String keystore_password;
	
	public static void main(String [] args) {
		try{
			logger.info ("In main() - " + args[0]);
			args_cpy = args;
			handler = new AuditHandler();
			processor = new AuditEvent.Processor(handler);
			config_handler = new ConfigHandler();
			config_processor = new Config.Processor(config_handler);
			
			
	        String userName = "root";
	        String password = args[1];
	        String url = "jdbc:mysql://localhost/auditcloud?zeroDateTimeBehavior=convertToNull";
	        keystore_password = args[2];

	        DbUtils.loadDriver("com.mysql.jdbc.Driver");
	        
	        BoneCPConfig config = new BoneCPConfig();	// create a new configuration object
	     	config.setJdbcUrl(url);			// set the JDBC url
	    	config.setUsername(userName);	// set the username
	    	config.setPassword(password);	// set the password
	    	
	    	connectionPool = new BoneCP(config); 	// setup the connection pool

	        
	        logger.info ("Database connection established");
			
			
			Configuration conf = HBaseConfiguration.create(); 
			// conf.set("hbase.zookeeper.quorum", "server1,server2,server3");
	
			table = new HTable(conf, "sql_audit"); 
			Runnable simple = new Runnable() {
	          public void run() {
	            simple(processor, args_cpy[0]);
	          }
	         };
			Runnable configServer = new Runnable() {
		          public void run() {
		        	  configServer(config_processor, args_cpy[0]);
		          }
		         };	
		         
		Runnable heartBeat = new Runnable() {
	          public void run() {
	        	  checkHeartBeat();
	          }
	         };	     
	      new Thread(simple).start();
	      new Thread(configServer).start();
	      new Thread(heartBeat).start();
	      
	      
		}catch(Exception e){
			e.printStackTrace();
		}
	}
	
	public static void configServer(Config.Processor cp, String iname){
		 try {	
	    	TSSLTransportFactory.TSSLTransportParameters params = new TSSLTransportFactory.TSSLTransportParameters();
	    	params.setKeyStore("keystore.jks", keystore_password);
	    	TServerSocket serverTransport_config = TSSLTransportFactory.getServerSocket(7912, 10000, InetAddress.getByName(iname), params);
	    	TServer server_config = new TThreadPoolServer(new TThreadPoolServer.Args(serverTransport_config).processor(config_processor));
	
	    	logger.info ("Starting server on port 7912 ...");
	    	server_config.serve();

	    } catch (Exception e) {
	      e.printStackTrace();
	    }
	}
	  public static void simple(AuditEvent.Processor processor, String iname) {
		    try {		    	
		    	//TODO Ensure performance of SSL is good
		    	TSSLTransportFactory.TSSLTransportParameters params = new TSSLTransportFactory.TSSLTransportParameters();
		    	params.setKeyStore("keystore.jks", keystore_password);
		    	
		    	TServerSocket serverTransport = TSSLTransportFactory.getServerSocket(7911, 10000, InetAddress.getByName(iname), params);
		    	TServer server = new TThreadPoolServer(new TThreadPoolServer.Args(serverTransport).processor(processor));
		    	
		    	
		    	logger.info ("Starting server on port 7911 ...");
		    	server.serve();

		    } catch (Exception e) {
		      e.printStackTrace();
		    }
	   }
	  
	  public static void checkHeartBeat(){
		  while (true){
			  try {
				  Thread.sleep(60*1000*8);
				  logger.info ("Checking heart beat");
				  AuditHandler.checkHeartBeat();
			   } catch (InterruptedException e) {
					logger.error("Unable to sleep on checkHeartBeat");
			   }
			  
		  }
	  }
}
