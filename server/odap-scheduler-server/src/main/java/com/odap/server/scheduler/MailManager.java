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

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.Properties;

import javax.mail.Address;
import javax.mail.Message;
import javax.mail.MessagingException;
import javax.mail.PasswordAuthentication;
import javax.mail.Session;
import javax.mail.Transport;
import javax.mail.internet.AddressException;
import javax.mail.internet.InternetAddress;
import javax.mail.internet.MimeMessage;

import org.apache.log4j.Logger;

import com.odap.common.util.LogUtil;

public class MailManager {

	static Logger logger = LogUtil.getInstance();
	
	private static Message getMessage(){
		Properties props = new Properties();
		props.put("mail.smtp.host", "smtp.gmail.com");
		props.put("mail.smtp.socketFactory.port", "465");
		props.put("mail.smtp.socketFactory.class",
				"javax.net.ssl.SSLSocketFactory");
		props.put("mail.smtp.auth", "true");
		props.put("mail.smtp.port", "465");
		
		Session session = Session.getDefaultInstance(props,
				new javax.mail.Authenticator() {
					protected PasswordAuthentication getPasswordAuthentication() {
						return new PasswordAuthentication("db2robw@gmail.com","ocolrnoimnjsimtq");
					}
		});
		return new MimeMessage(session);
	}
	
	public static void sendNotifications(String addTitle, String addBody, List<Map<String, Object>> notifications){
		logger.info("Entering sendNotifications()");
		Address[] recipients = new Address[notifications.size()];
		String title = "";
		String body = "";
		String server_name = "";
		
		int i = 0;
		for(Map<String, Object> notify : notifications ){
			try {
				title = notify.get("title").toString();
				body = notify.get("body").toString();
				server_name = notify.get("server_name").toString();
				recipients[i++] = new InternetAddress(notify.get("email").toString());
				logger.info("Sending email to:" + notify.get("email").toString());
			} catch (AddressException e) {
				logger.error("Invalid address:", e);
			}
		}
		
		try{
			Message message = getMessage();
			message.setFrom(new InternetAddress("db2robw@gmail.com"));
			message.setRecipients(Message.RecipientType.TO,
					recipients);
			
			message.setSubject(title + addTitle + server_name);
			message.setText(body + addBody);
			
			logger.info("Email title: " + title + addTitle);
			logger.info("Email body:" + body + addBody);
			
			Transport.send(message);
		}catch(Exception e){
			logger.error("Problem sending notification",e);
		}
		
		logger.info("Exiting sendNotification");
	}
	
	public static void main(String[] args) {
		
		try {
 
			Message message = getMessage();
			message.setFrom(new InternetAddress("db2robw@gmail.com"));
			message.setRecipients(Message.RecipientType.TO,
					InternetAddress.parse("db2robw@gmail.com"));
			message.setSubject("Testing Subject");
			message.setText("Dear Mail Crawler," +
					"\n\n No spam to my email, please!");
 
			Transport.send(message);
 
			System.out.println("Done");
 
		} catch (MessagingException e) {
			throw new RuntimeException(e);
		}
		
		
		

	}

}
