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
cd ../Open-Database-Audit-Project/client
make clean
./configure --bindir=/usr/bin --sysconfdir=/etc prefix=
make -j6

%install
cd ../Open-Database-Audit-Project/client
make DESTDIR=${RPM_BUILD_ROOT} install 


%clean
if [ "$RPM_BUILD_ROOT" != "/" ]; then
        rm -rf $RPM_BUILD_ROOT
fi

%files
%defattr(-,root,root)
/usr/bin/odap
/etc/odap/certificate.pem
/etc/odap/log4j.properties
/etc/init.d/odap

%preun  
rm -f /usr/bin/odap
rm -f /etc/init.d/odap
rm -rf /etc/odap
exit 0
