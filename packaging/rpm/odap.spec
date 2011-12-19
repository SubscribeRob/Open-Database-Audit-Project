#%define _topdir	 	/home/rob/source
%define name		odap-client
%define release		1
%define version 	0.9
%define buildroot %{_topdir}/%{name}-%{version}-root

Name:           %{name}
Release:        %{release}
Version:	%{version}
Summary:        Open Database Audit Project client capture program
BuildRoot:	${buildroot}

Group:          Database/Auditing
License:        GNU/GPL
URL:            http://www.opendbaudit.com

%description
Database Auditing Software

%build
cd client
make
cd ..
cd kernel/linux
make

%install
cd client
make install prefix=$RPM_BUILD_ROOT/opt/odap
cd ..
cd kernel/linux
make install prefix=$RPM_BUILD_ROOT

%files
%defattr(-,root,root)
/opt/odap/bin/odap
/opt/odap/bin/certificate.pem
/opt/odap/bin/log4j.properties
/opt/odap/kernel/odap_monitor.ko
/etc/init.d/odap
/opt/odap/bin/odap-daemon
