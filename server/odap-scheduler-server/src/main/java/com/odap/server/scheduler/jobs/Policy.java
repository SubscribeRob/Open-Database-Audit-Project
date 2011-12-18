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
package com.odap.server.scheduler.jobs;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.math.BigInteger;
import java.security.SecureRandom;
import java.util.List;
import java.util.Map;

import org.apache.log4j.Logger;
import org.quartz.Job;
import org.quartz.JobDataMap;
import org.quartz.JobExecutionContext;
import org.quartz.JobExecutionException;

import com.odap.server.scheduler.*;
import com.odap.common.util.*;
import com.odap.common.hbase.*;

public class Policy implements Job {
	static Logger logger = LogUtil.getInstance();
	static private SecureRandom random = new SecureRandom();
	
	private static String nextSessionId()  {
	    return new BigInteger(130, random).toString(16);
	}
	
	public void execute(JobExecutionContext context) throws JobExecutionException {
		logger.info("Entering Policy:execute()");
		try {
			JobDataMap data = context.getJobDetail().getJobDataMap();
			
			String sid = nextSessionId();
			String server_id = data.get("server_id").toString();
			String job_id = data.get("job_id").toString();
			String program = createProgram((Map<String, Object>)data.get("row"));
			List<Map<String, Object>> notifications = (List<Map<String, Object>>)data.get("notifications");
			
            String export_compress_command = "/opt/auditcloud/bin/policy_and_count.sh " + server_id + " 0 2147483647 /policy/server_" + server_id  + sid  + " " + program + "";
            logger.info("Executing " + export_compress_command);
			Process p = Runtime.getRuntime().exec(export_compress_command);
			
			
	        BufferedReader stdInput = new BufferedReader(new InputStreamReader(p.getInputStream()));
            BufferedReader stdError = new BufferedReader(new InputStreamReader(p.getErrorStream()));
            String s = "";
            
            // read any errors from the attempted command
            while ((s = stdError.readLine()) != null) {
                logger.info(s);
            }
			
            // read the output from the command
            String prev_line = "";
            while ((s = stdInput.readLine()) != null) {
                logger.info(s);
                prev_line = s;
            }
            
            int rc = p.waitFor();
            
            logger.info("Number of violaitons:" + prev_line + ":");
            rc = Integer.parseInt(prev_line);

            
            
            if(rc > 0){
            	logger.info("Sending notification");
            	
            	JobManager.createWebNotification("ALERT", data.get("account_id").toString(), "The policy " + ((Map<String, Object>)data.get("row")).get("policy_name") + " had " + rc + " violations" );
            	
            	MailManager.sendNotifications("", "\n\nThere were " + rc + " violations of the policy", notifications);

            }
            
			
		}catch(Exception e){
			logger.error("Policy failure",e);
		}
		
		
		logger.info("Exiting Policy:execute()");
	}
	
	private String createProgram(Map<String, Object> row){
		String program = "";
		
		program += createFilter("TABLE",row.get("tables").toString());
		program += createFilter("LOGIN",row.get("users").toString());
		program += createFilter("IP",row.get("ips").toString());
		program += createFilter("PORT",row.get("ports").toString());
		program += createFilter("COLUMN",row.get("columns").toString());
		
		if(program.length() > 0){
			program = program.substring(0, program.length()-1);
		}
		logger.info("Program create: " + program);
		return program;
	}
	
	private static String createFilter(String key, String value){
		if(value != null && value.length() > 0){
			return key +  " " +  value + " &";
		}else{
			return "";
		}
	}

}
