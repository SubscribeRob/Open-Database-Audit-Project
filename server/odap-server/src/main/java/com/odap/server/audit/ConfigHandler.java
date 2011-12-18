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

import com.odap.common.thrift.*;
import com.odap.common.util.LogUtil;

import java.math.BigInteger;
import java.security.MessageDigest;
import java.security.SecureRandom;
import java.sql.SQLException;
import java.util.List;
import java.util.Map;

import org.apache.commons.codec.digest.DigestUtils;
import org.apache.commons.dbutils.QueryRunner;
import org.apache.commons.dbutils.handlers.MapListHandler;
import org.apache.log4j.Logger;
import org.apache.thrift.TException;

public class ConfigHandler implements Config.Iface{
	
	static private SecureRandom random = new SecureRandom();
	static Logger logger = LogUtil.getInstance();
	
	
	private static String nextSessionId()  {
	    return new BigInteger(130, random).toString(16);
	}
	


	public ConfigMessage registerNewServer(ConfigMessage config)
			throws TException {
		logger.info("Enter registerNewServer()");
		ConfigMessage msg = new ConfigMessage();
		
		QueryRunner qRunner = new QueryRunner();
		
		Integer account_id = null;
		Integer server_id = null;
		//Authenticate the user and get the account_id;
		String query = "SELECT * FROM joomla.cloud_users WHERE username = ? AND password = ?";
		{
			String parameters[] = { config.getUsername(), DigestUtils.md5Hex(config.getPassword())};
			try {
				List<Map<String,Object>> mapList = (List<Map<String, Object>>) qRunner.query(AuditServer.conn,query,new MapListHandler(),parameters);
				
				if(mapList.size() < 1){
					logger.warn("Username " + config.getUsername() + " not authenticated");
					return msg;
				}
				
				account_id = (Integer) mapList.get(0).get("account_id");
				
			} catch (SQLException e) {
				logger.error("Issue finding user account",e);
				return msg;
			}  
		}
		
		String session_id = nextSessionId();
		{
			try {
					{
						query = "INSERT INTO servers (account_id,server_name,server_software,server_port,server_authentication_token,server_timezone,strip_predicates) VALUES (?,?,?,?,?,?,?)";
						Object parameters[] = {account_id.toString(),config.getServer_name(),config.getServer_software(),new Short(config.getPort()),session_id,new Double(config.getTimezone_offset()),config.strip_predicates};
						qRunner.update( AuditServer.conn,query, parameters);
					}{
						String parameters[] = {account_id.toString(),config.getServer_name(),session_id};
						query = "SELECT * FROM servers WHERE account_id = ? AND server_name = ? and server_authentication_token = ?";
						List<Map<String,Object>> mapList = (List<Map<String, Object>>) qRunner.query(AuditServer.conn,query,new MapListHandler(),parameters);
						
						if(mapList.size() < 1){
							logger.error("Unable to find server after after registering it");
							return msg;
						}
						
						server_id = (Integer) mapList.get(0).get("id");
					}
			} catch (SQLException e) {
				logger.error("Issue registering server",e);
			}
		}
		
		msg.token = session_id;
		msg.server_id = server_id.shortValue();
		msg.server = "dbauditcloud.com";
		logger.info("Exiting registerNewServer()");
		return msg;
	}



	@Override
	public ConfigMessage getConfig(ConfigMessage config) throws TException {
		// TODO Auto-generated method stub
		return null;
	}

}
