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

#include "protocol/AuditEvent.h"  // As an example

#include <transport/TSocket.h>
#include <transport/TSSLSocket.h>
#include <transport/TSSLServerSocket.h>
#include <transport/TBufferTransports.h>
#include <protocol/TBinaryProtocol.h>
#include <boost/shared_ptr.hpp>
#include <boost/tokenizer.hpp>
#include <pcap.h>
#include <stdio.h>
#include <string.h>
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include "include/charsets.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctime>
#include <sstream>
#include <list>
#include "CachedUserManager.h"
#include "ConfigFile.h"
#include "protocol/Config.h"
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace std;
#include <unistd.h>
#include <curses.h>
#include "log4cxx/logger.h"
#include "log4cxx/basicconfigurator.h"
#include "log4cxx/helpers/exception.h"
#include "capture/database/DB2Connection.h"
#include "capture/database/MySQLConnection.h"
#include "log4cxx/propertyconfigurator.h"
#include <fcntl.h>
#include <signal.h>
#include "Threads.h"
#include <boost/thread.hpp>
using namespace log4cxx;
using namespace log4cxx::helpers;

#include <boost/regex.hpp>
#include <stdexcept>

int tcpcap();
void devread();

int server_id = -1;
std::string token = "ABCDEFG";
bool strip_predicates = false;

AuditEventClient * client_store = NULL;
ConfigClient * config_client = NULL;

CachedUserManager userManager;
LoggerPtr logger(Logger::getLogger("main"));


string kernel_module;
string kernel_module_name;
string insmod_cmd;
string rmmod_cmd;
string database_type;
string server_port;

using namespace boost;

class AuditCloudAccessManager: public AccessManager {
 public:
  // AccessManager interface
  Decision verify(const sockaddr_storage& sa) throw(){ return AccessManager::ALLOW; }
  Decision verify(const std::string& host, const char* name, int size) throw(){ return AccessManager::ALLOW; }
  Decision verify(const sockaddr_storage& sa, const char* data, int size) throw(){ return AccessManager::ALLOW; }
};

ConfigFile * config;

char* findBinPath(const char* argv)
{
	static char path[300];
	int i;

	strncpy(path, argv, sizeof(path));

	for (i = strlen(path) - 1; i >= 0; --i)
	{
		if (path[i] == '\\' || path[i] == '/')
		{
			path[i + 1] = 0;
			break;
		}
	}

	if (i == -1) // user just typed the binary name
		strcpy(path, ".");

	return path;
}

void terminate(int param){
	LOG4CXX_DEBUG(logger,"Terminating program");
	  LOG4CXX_DEBUG(logger,"Unload module:" << (rmmod_cmd +  " " + kernel_module_name).c_str());
	  system((rmmod_cmd + " " + kernel_module_name).c_str());
	exit (0);

}

bool runConfig(char * bin_path){

	  LOG4CXX_DEBUG(logger,"Entering runConfig()");

	  string username;
	  do{
		  cout << "Enter your ODAP username (Website username):";
	  	  cin >> username;

	  }while(username.size() < 1);

	  string password;

	  do{
		  cout << "Enter your ODAP password:";
		  system("stty raw -echo");
		  cin >> password;
		  system("stty cooked echo");
		  cout << endl;
	  }while(password.size() < 1);

	  //If new install then register new server
	  string server_name;

	  while(true){
		  cout << "Enter a server name for which you will use to reference this server in the UI:";
	  	  cin >> server_name;
	  	  if(server_name.size() < 1){
	  		  cout << "Please enter a valid server name" << endl;
	  		  continue;
	  	  }else{
	  		  break;
	  	  }
	  }

		  //If new install then register new server
		  string server_software;
		  while(true){
			  cout << "What server software do you want to audit? Enter 1 for MySQL, 2 for DB2, 3 for PostgreSQL, 4 for Oracle:";
			  cin >> server_software;

			  if(server_software.compare("1") == 0){
				  server_software = "MySQL";
			  }else if(server_software.compare("2") == 0){
				  server_software = "DB2";
			  }else if(server_software.compare("3") == 0){
				  server_software = "PostgreSQL";
			  }else if(server_software.compare("4") == 0){
				  server_software = "Oracle";
			  }else{
				  cout << "Unknown server software... Please choose one of the options"<<endl;
				  continue;
			  }
			  break;
		  }

		  string strip_predicates;

		  while(true){
			  cout << "Would you like predicates stripped from SQL and not transmitted (Ex WHERE id = ? instead of WHERE id = 2): [y/n] ";
		  	  cin >> strip_predicates;

		  	  if(strip_predicates.compare("y") != 0 && strip_predicates.compare("Y") != 0
		  		&& strip_predicates.compare("n") != 0  && strip_predicates.compare("N") != 0){
		  		  cout << "Enter either y or n" << endl;
		  	  }else{
		  		  break;
		  	  }
		  }

		  while(true){
			  cout << "What port does the server listen on (number only and enter 1 if it does not listen on TCP/IP)? ";
		  	  cin >> server_port;
		  	  if(atoi(server_port.c_str()) < 1){
		  		  cout << "Enter a valid numeric port" << endl;
		  	  }else{
		  		  break;
		  	  }
		  }


		  string timezone_offset;

		  while(true){
			  cout << "Enter your GMT offset(Ex: -5 for EST or -3.5 for Newfoundland): ";
		  	  cin >> timezone_offset;

		  	  if(atof(timezone_offset.c_str()) == 0.0 && timezone_offset.compare("0.0") != 0){
		  		  cout << "Enter a valid numeric offset" << endl;
		  	  }else{
		  		  break;
		  	  }
		  }

		  LOG4CXX_DEBUG(logger,"About to register new server()");

		  ConfigMessage config_message;
		  ConfigMessage config_message_return;

		  config_message.username = username;
		  config_message.password = password;
		  config_message.server_software = server_software;
		  config_message.port = atoi(server_port.c_str());
		  config_message.server_name = server_name;
		  config_message.timezone_offset = atof(timezone_offset.c_str());

		  if(strip_predicates.compare("y") == 0){
			  config_message.strip_predicates = true;
		  }else{
			  config_message.strip_predicates = false;
		  }

		  config_client->registerNewServer(config_message_return,config_message);
		  LOG4CXX_DEBUG(logger,"Register new server returned()");
		  if(config_message_return.server_id < 1 || config_message_return.token.size() < 1){
			  LOG4CXX_FATAL(logger,"Unable to register server. Username and or password is incorrect");
			  return false;
		  }
		  config->add("server_id",config_message_return.server_id);
		  config->add("token",config_message_return.token);
		  config->add("type",server_software);
		  config->add("port",server_port);
		  config->add("strip_predicates",strip_predicates);
		  config->add("server",config_message_return.server);
		  token = config_message_return.token;
		  server_id = config_message_return.server_id;


	  //Now store the config file
	  ofstream config_file;
	  config_file.open ((bin_path + string("config->ini")).c_str());

	  config_file <<config;
	  config_file.close();

	  cout << "To start db auditing as root execute \"service odap start\"" << endl;

	  LOG4CXX_DEBUG(logger,"Exiting runConfig()");
	  return true;
}
int main(int argc, char **argv) {

	if(argc == 3){
		config = new ConfigFile( argv[2] );
	}else{
		config = new ConfigFile( "config.ini" );
	}
	setuid(0);
	PropertyConfigurator::configure((findBinPath(argv[0]) + string("log4j.properties")).c_str());
	LOG4CXX_DEBUG(logger,"Entering main()");

	shared_ptr<TSSLSocketFactory> factory(new TSSLSocketFactory());
	factory->ciphers("ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");
	factory->loadTrustedCertificates((findBinPath(argv[0]) + string("certificate.pem")).c_str());
	factory->authenticate(true);
	shared_ptr<AccessManager> am (new AuditCloudAccessManager);
	factory->access(am);

	string remote_server;

	config->readInto(remote_server, "server" , string("opendbaudit.com"));
	config->readInto(server_id, "server_id" , -1);
	config->readInto(strip_predicates, "strip_predicates" , false);
	config->readInto(server_port, "port" , string("-1"));

	config->readInto(kernel_module, "kernel_module" , string("/opt/odap/bin/odap_monitor.ko"));
	config->readInto(kernel_module_name,"kernel_module_name",string("odap_monitor"));
	config->readInto(insmod_cmd, "insmod_cmd" , string("/sbin/insmod"));
	config->readInto(rmmod_cmd, "rmmod_cmd" , string("/sbin/rmmod"));
	config->readInto(database_type,"type" , string("-1"));


	shared_ptr<TSocket> socket_config = factory->createSocket(remote_server,7912);
	boost::shared_ptr<TTransport> transport_config(new TBufferedTransport(socket_config));

	boost::shared_ptr<TProtocol> protocol_config(new TBinaryProtocol(transport_config));
	config_client = new ConfigClient(protocol_config);

	transport_config->open();
	if(argc == 2 || server_id == -1){
		runConfig(findBinPath(argv[0]));
		transport_config->close();
		return 0;
	}else{
		// Load config from server
	}


	transport_config->close();

	{
		  string type = "0";

		  if(strcmp("DB2",database_type.c_str()) == 0){
			  type = "1";
		  }

		  LOG4CXX_DEBUG(logger,"insmod:" << (insmod_cmd +  " " + kernel_module+ " mode=" + type).c_str());
		  system((insmod_cmd + " " + kernel_module+ " mode=" + type).c_str());

		  signal(SIGTERM,terminate);
		  signal(SIGINT,terminate);
		  signal(SIGSEGV,terminate);
		  signal(SIGILL,terminate);
	}


	shared_ptr<TSocket> socket = factory->createSocket(remote_server,7911);
	boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
	boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
	client_store = new AuditEventClient(protocol);
	transport->open();

	boost::thread SendMessageThread(sendMessageThread);
	boost::thread KernelThread(devread);
	boost::thread TCPThread(tcpcap);

	TCPThread.join();
	KernelThread.join();
	SendMessageThread.join();
	transport->close();

  LOG4CXX_DEBUG(logger,"Exiting main()");
  return 0;
}

void devread(){

	  int fd = open("/dev/odap",  O_RDONLY);

	  char buffer[1024*1024];	//allocate 64k buffer

	  MySQLConnection mysql;
	  DB2Connection db2;
	  ssize_t total_read;

	  int type = 0;

	  if(strcmp("DB2",database_type.c_str()) == 0){
		  type = 1;
	  }

	  LOG4CXX_DEBUG(logger,"Parsing type" << type);
	  while((total_read = read(fd,buffer,1024*256)) != 0){
		  if(total_read < 1){
			  LOG4CXX_ERROR(logger,"Read returned -1");
			  return;
		  }

		  LOG4CXX_DEBUG(logger,"Line to process" << buffer);
		  if(type == 0){
			  mysql.parse(&buffer[0],total_read);
		  }else if (type == 1){
			  db2.parse_format_2(&buffer[0],total_read);
		  }

	  }
	  close(fd);

}

int tcpcap() {
  pcap_t *handle;/* Session handle */
  char *dev = "any";/* The device to sniff on */
  char errbuf[PCAP_ERRBUF_SIZE];/* Error string */
  struct bpf_program fp;/* The compiled filter */
  int res;
  const u_char *pkt_data;

  char filter_exp[64] ;/* The filter expression */
  strcpy(filter_exp,"dst port ");
  strcat(filter_exp,server_port.c_str());


 // strcat(filter_exp, " or src port ");
 // strcat(filter_exp,argv[2]);

  bpf_u_int32 mask;/* Our netmask */
  bpf_u_int32 net;/* Our IP */
  struct pcap_pkthdr * header = new pcap_pkthdr;/* The header that pcap gives us */
  const u_char *packet;/* The actual packet */

  /* Find the properties for the device */
  if (pcap_lookupnet(dev, &net, &mask, errbuf) == -1) {
    fprintf(stderr, "Couldn't get netmask for device %s: %s", dev, errbuf);
    net = 0;
    mask = 0;
    }

   /* Open the session in promiscuous mode */
  handle = pcap_open_live(dev, BUFSIZ, 1, NULL, errbuf);
  if (handle == NULL) {
    fprintf(stderr, "Couldn't open device %s: %s", dev, errbuf);
    return(2);
  }

  /* Compile and apply the filter */
  if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1) {
    fprintf(stderr, "Couldn't parse filter %s: %s", filter_exp, pcap_geterr(handle));
    return(2);
  }

  if (pcap_setfilter(handle, &fp) == -1) {
    fprintf(stderr, "Couldn't install filter %s: %s", filter_exp, pcap_geterr(handle));
    return(2);
  }


  DB2Connection db2;
  MySQLConnection mysql;

  while((res = pcap_next_ex( handle, &header, &pkt_data)) >= 0){

      if(res == 0)
          /* Timeout elapsed */
          continue;
      	  //

      if(strcmp("DB2",database_type.c_str()) == 0){
      	  db2.parse(pkt_data,header,2);
      }else{
      	  mysql.parse(pkt_data,header,2);
      }

  }
  /* And close the session */
  pcap_close(handle);
  return(0);
}
