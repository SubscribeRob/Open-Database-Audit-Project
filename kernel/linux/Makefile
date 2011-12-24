obj-m := odap_monitor.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
clean:
	rm -f *.o *.ko *.unsigned *.symvers *.mod.c *.order
install:
	install --directory   $(prefix)/opt/odap/bin
	install --directory   $(prefix)/opt/odap/kernel
	install --directory   $(prefix)/opt/odap/logs
	install --directory   $(prefix)/etc/init.d/
	/usr/bin/install -c odap_monitor.ko $(prefix)/opt/odap/kernel/
	/usr/bin/install -c ../../config/linux/odap $(prefix)/etc/init.d/
	/usr/bin/install -c ../../config/certificate.pem $(prefix)/opt/odap/bin/
	/usr/bin/install -c ../../config/log4j.properties $(prefix)/opt/odap/bin/	
