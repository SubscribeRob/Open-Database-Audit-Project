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
import java.sql.Date;
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

public class DownloadServerData implements Job {

	static Logger logger = LogUtil.getInstance();
	static private SecureRandom random = new SecureRandom();
	
	private static String nextSessionId()  {
	    return new BigInteger(130, random).toString(16);
	}
	
	public void execute(JobExecutionContext context) throws JobExecutionException {
		logger.info("Entering DownloadServerData:execute()");
		
		try {
			JobDataMap data = context.getJobDetail().getJobDataMap();
			
			String sid = nextSessionId();
			String server_id = data.get("server_id").toString();
			String job_id = data.get("job_id").toString();
			List<Map<String, Object>> notifications = (List<Map<String, Object>>)data.get("notifications");
			Map<String,Object> row = (Map<String, Object>)data.get("row");
			
			
			String start_time = "";
			String end_time = "";
			
			if(row.get("start_date") != null && row.get("start_date").toString().length() > 0){
				Date tmp = (Date)row.get("start_date");
				start_time = new Long(tmp.getTime()/1000).toString();
				tmp = (Date)row.get("end_date");
				end_time = new Long(tmp.getTime()/1000).toString();
			}
			
            String export_compress_command = "/opt/auditcloud/bin/export_part_folder_and_compress.sh " + server_id + " /export/server_" + server_id  + sid  + " /opt/auditcloud/export/server_" + server_id  + sid + ".gz" + " " + start_time + " " + end_time;
            logger.info("Executing " + export_compress_command);
			Process p = Runtime.getRuntime().exec(export_compress_command);
			
			
	        BufferedReader stdInput = new BufferedReader(new InputStreamReader(p.getInputStream()));
            BufferedReader stdError = new BufferedReader(new InputStreamReader(p.getErrorStream()));
            String s = "";

            // read the output from the command
            while ((s = stdInput.readLine()) != null) {
                logger.info(s);
            }
            
            // read any errors from the attempted command
            while ((s = stdError.readLine()) != null) {
                logger.info(s);
            }
			
   
        	JobManager.createWebNotification("INFO", data.get("account_id").toString(), "Export request for server " + data.get("server_name").toString() + " ready for download" );


            MailManager.sendNotifications("", " http://localhost/export/server_" + server_id  + sid + ".gz", notifications);
            
            //Now remove the job from the job queue
            JobManager.removeJob(job_id);
            
	          
		} catch (Exception e) {
			logger.error("Exception when trying to export data",e);
		}
		
		logger.info("Exiting DownloadServerData:execute()");
	}

}
