# Tools
ifndef THRIFT
THRIFT = /usr/local/bin/thrift
endif # THRIFT

stubs: AuditEvent.thrift UICommProtocol.thrift
	$(THRIFT) --gen cpp  AuditEvent.thrift
	$(THRIFT) --gen java AuditEvent.thrift
	$(THRIFT) --gen java UICommProtocol.thrift
	$(THRIFT) --gen php  UICommProtocol.thrift

clean:
	rm -rf gen-cpp gen-php gen-java
