bin_PROGRAMS = odap

AM_CXXFLAGS = $(INTI_CFLAGS) -I/usr/local/include/log4cxx -I/usr/local/include/boost

odap_SOURCES = CachedUserInfo.h CachedUserManager.h ConfigFile.h  EventQueue.h main.cpp  Threads.cpp CachedUserInfo.cpp  \
	       CachedUserManager.cpp  ConfigFile.cpp EventQueue.cpp Threads.h capture/database/DatabaseConnection.cpp  \
	       capture/database/DatabaseConnection.h  capture/database/DB2Connection.cpp  capture/database/DB2Connection.h \
	       capture/database/MySQLConnection.cpp  capture/database/MySQLConnection.h include/charsets.cpp  include/charsets.h \
	       protocol/Config.cpp  protocol/Config.h protocol/AuditEvent.cpp protocol/AuditEvent.h \
	       protocol/auditevent_constants.cpp protocol/auditevent_types.cpp
odap_LDADD = -lpcap -lrt -lssl -lcrypto -laprutil-1 -lapr-1 $(BOOST_THREAD_LIBS) $(BOOST_REGEX_LIBS) $(BOOST_THREAD_LIBS)
odap_DATA = certificate.pem log4j.properties
odapdir=$(sysconfdir)/odap
odap_init_d_SCRIPTS = init.d/odap
odap_init_ddir=$(sysconfdir)/init.d
odap_CPPFLAGS = $(BOOST_CPPFLAGS) -DSYSCONFDIR="\"$(odapdir)\"" -DSYSBINDIR="\"$(bindir)\""
odap_LIBS = $(BOOST_THREAD_LIBS)
