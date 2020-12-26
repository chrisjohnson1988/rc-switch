all: send send_raw

send: RCSwitch.o send.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $+ -o $@ -lwiringPi

send_raw: send_raw.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $+ -o $@
	
clean:
	$(RM) *.o send send_raw
