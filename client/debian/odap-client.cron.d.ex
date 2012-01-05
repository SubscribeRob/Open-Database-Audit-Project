#
# Regular cron jobs for the odap-client package
#
0 4	* * *	root	[ -x /usr/bin/odap-client_maintenance ] && /usr/bin/odap-client_maintenance
