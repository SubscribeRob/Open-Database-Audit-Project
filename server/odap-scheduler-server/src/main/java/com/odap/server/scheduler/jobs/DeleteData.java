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

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.hbase.HBaseConfiguration;
import org.apache.hadoop.hbase.client.Delete;
import org.apache.hadoop.hbase.client.HTable;
import org.apache.hadoop.hbase.client.Result;
import org.apache.hadoop.hbase.client.ResultScanner;
import org.apache.hadoop.hbase.client.Scan;
import org.apache.log4j.Logger;
import org.quartz.Job;
import org.quartz.JobDataMap;
import org.quartz.JobExecutionContext;
import org.quartz.JobExecutionException;

import com.odap.server.scheduler.*;
import com.odap.common.util.*;
import com.odap.common.hbase.*;
public class DeleteData implements Job {
	static Logger logger = LogUtil.getInstance();
	
	public void execute(JobExecutionContext context) throws JobExecutionException {
		logger.info("Entering Delete:execute()");
		Configuration conf = HBaseConfiguration.create(); 
		try {
			HTable table = new HTable(conf, "sql_audit");
			
			JobDataMap data = context.getJobDetail().getJobDataMap();
			String server_id = data.get("server_id").toString();
			List<Map<String, Object>> notifications = (List<Map<String, Object>>)data.get("notifications");
			
			 Scan scan1 = EventHBaseHandler.getStartStopFilter((short)Short.parseShort(server_id),0,Integer.MAX_VALUE);
			 ResultScanner scanner1 = table.getScanner(scan1);
			 List<Delete> deletes = new LinkedList<>();
			 for(Result res: scanner1){
				 deletes.add(new Delete(res.getRow()));
				 if(deletes.size() > 10000){
					 logger.info("Deleting 10000 rows");
					 table.delete(deletes);
					 deletes.clear();
				 }
			 }
			 logger.info("Deleting " + deletes.size() + " rows");
			 table.delete(deletes);
			 
			 JobManager.createWebNotification("INFO", data.get("account_id").toString(), "Clean up for server " + data.get("server_name").toString() + " complete" );
	         JobManager.removeJob(data.get("job_id").toString());			 
			 
		} catch (IOException e) {
			logger.error("Delete job failed",e);
		}
		
		logger.info("Exiting Delete:execute()");
	}
}
