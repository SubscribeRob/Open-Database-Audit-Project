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

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import org.apache.commons.dbutils.DbUtils;
import org.apache.commons.dbutils.QueryRunner;
import org.apache.commons.dbutils.handlers.MapListHandler;
import org.apache.log4j.Logger;
import org.quartz.Job;
import org.quartz.JobDetail;
import org.quartz.JobKey;
import org.quartz.Scheduler;
import org.quartz.SchedulerException;
import org.quartz.SimpleTrigger;


import static org.quartz.JobBuilder.newJob; 
import static org.quartz.TriggerBuilder.*;
import static org.quartz.SimpleScheduleBuilder.*;
import static org.quartz.CronScheduleBuilder.*;
import static org.quartz.DateBuilder.*;
	
//Called on start up of scheduler and every x minutes to refresh the scheduled jobs
//keeps track 
public  class JobManager {

	static Logger logger = Logger.getLogger(JobManager.class);
	
	static List<Map<String,Object>> scheduledMapList = new LinkedList<> (); //Holds a copy of the jobs table in memory
	
	static Scheduler sched = null;
	
	static Connection conn = null;
	
	int future_offset = 5;
	
	public JobManager(){
		logger.info("Entering JobManager()");
		
		try{
			sched = new org.quartz.impl.StdSchedulerFactory().getScheduler();
			sched.start();
		}catch(Exception e){
			logger.error("Unable to create scheduler",e);
		}
		
        String userName = "root";
        String password = "";
        String url = "jdbc:mysql://localhost/auditcloud?zeroDateTimeBehavior=convertToNull";
        
        DbUtils.loadDriver("com.mysql.jdbc.Driver");
        try {
			conn = DriverManager.getConnection (url, userName, password);
		} catch (SQLException e) {
			logger.error("Unable to connect to mysql",e);
		}
        
        logger.info ("Database connection established");
		logger.info("Exiting JobManager()");
	}
	
	
	
	public synchronized List<Map<String,String>> refreshJobs(){
		
		logger.info("Entering refreshJobs()");
		
        try{
            QueryRunner qRunner = new QueryRunner();
            
            List<Map<String,Object>> mapList = (List<Map<String, Object>>) qRunner.query(conn, "SELECT jobs.*, servers.account_id,servers.server_name FROM jobs,servers where servers.id = jobs.server_id", new MapListHandler());


            processJobs(mapList);


        }catch (Exception e){
            System.err.println ("Cannot connect to database server");
            e.printStackTrace();
        }
        
        logger.info("Exiting refreshJobs()");
        return null;
	}
	
	
	private synchronized void processJobs(List<Map<String,Object>> newJobList){
		logger.info("Entering processJobs()");
		 future_offset = 1;
		
			for(Map<String,Object> job : newJobList){
				logger.info("Searching for job id:" + job.get("id"));
				
				Object jobExists = null;
				for(Map<String,Object> scheduledJob : scheduledMapList){
					if(scheduledJob.get("id").toString().compareTo(job.get("id").toString()) == 0){
						jobExists = scheduledJob;
						break;
					}
				}
				
				if(jobExists == null){
					//Create new quartz job
					logger.info("Creating new job for " + job.get("id"));
					try {

						Class<? extends Job> jobClass = (Class<? extends Job>) Class.forName(job.get("pre_processor").toString());
						

						
						
						
							
							JobDetail quartzJob =  newJob(jobClass)
													.withIdentity("job-" + job.get("id"), "jobman")
													.build();
						
							quartzJob.getJobDataMap().put("server_id", job.get("server_id"));
							quartzJob.getJobDataMap().put("job_id", job.get("id"));
							quartzJob.getJobDataMap().put("account_id",job.get("account_id"));
							quartzJob.getJobDataMap().put("server_name",job.get("server_name"));
							quartzJob.getJobDataMap().put("row", job);
							
							QueryRunner qRunner = new QueryRunner();
							String sql = "select * from notifications n, joomla.cloud_users u, servers s, jobs j WHERE n.user_id = u.id and j.server_id = s.id AND job_id = " + job.get("id");
							logger.info("Getting notifications attached to job:" + sql);
							quartzJob.getJobDataMap().put("notifications", (List<Map<String, Object>>) qRunner.query(conn, sql, new MapListHandler()));

						// If the cron entry is empty/null then we schedule it as soon as possible to run
						// otherwise we schedule the cron job
						
						//TODO prevent flood of jobs on restart and workload management
						if(job.get("cron") == null || job.get("cron").toString().compareTo("") == 0){
							logger.debug("Scheduling one time execution for job " + job.get("id"));
							SimpleTrigger trigger = (SimpleTrigger) newTrigger() 
								    .withIdentity("trigger-" + job.get("id"), "onetime")
								    .startAt(futureDate(future_offset+=25, IntervalUnit.SECOND)) // some Date 
								    .forJob("job-" + job.get("id"), "jobman") // identify job with name, group strings
								    .build();
							
							sched.scheduleJob(quartzJob,trigger);
						}else{
							sched.scheduleJob(quartzJob,newTrigger()
							    .withIdentity("trig-" + job.get("id"), "jobman")
							    .withSchedule(cronSchedule(job.get("cron").toString()))
							    .forJob("job-" + job.get("id"), "jobman")
							    .build()
							 );
						}
						
					} catch (Exception e){
						logger.error("Unable to create job " + job.get("id"),e);
					}
				}
				
			}
			
			// Now deal with the case where jobs need to be descheduled as this is a second run
			for(Map<String,Object> scheduledJob : scheduledMapList){
				boolean job_found = false;
										
				for(Map<String,Object> newjob : newJobList){
					if(newjob.get("id").toString().compareTo(scheduledJob.get("id").toString()) == 0){
						job_found = true;
						break;
					}
				}
				if(job_found == false && scheduledJob.get("cron") != null && scheduledJob.get("cron").toString().length() > 0){
					logger.info("Need to unschedule job " + scheduledJob.get("id"));
					try {
						sched.deleteJob(new JobKey("job-" + scheduledJob.get("id"),"jobman"));
					} catch (SchedulerException e) {
						logger.error("Unable to remove job",e);
					}
				}
			}
		

			scheduledMapList = newJobList;
			
		logger.info("Exiting processJobs()");
	}
	
	public synchronized static void removeJob(String job_id){
		logger.info("Entering removeJob");
		try {
			String sql = "DELETE FROM jobs WHERE id = " + job_id;
			logger.info(sql);
			new QueryRunner().update(conn, sql );
		} catch (SQLException e) {
			logger.error("Error removing job", e);
		}
		logger.info("Exiting removeJob");
	}
	
	public synchronized static void createWebNotification(String type, String account_id,String message){
		logger.info("Entering removeJob");
		try {
			String sql = "INSERT INTO messages (message_type,message_text,account_id,message_timestamp) VALUES('" 
						+ type + "','" + message + "'," + account_id + ", UNIX_TIMESTAMP())";
			logger.info(sql);
			new QueryRunner().update(conn, sql );
		} catch (SQLException e) {
			logger.error("Error removing job", e);
		}
		logger.info("Exiting removeJob");
	}
}
