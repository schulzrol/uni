
all: hinfosvc.cpp
	g++ hinfosvc.cpp -ggdb -o hinfosvc

test: all
	killall -q hinfosvc ; true
	./hinfosvc 12345 &
	sleep 5
	curl -s localhost:12345/load ; echo
	curl -s localhost:12345/cpu-name ; echo
	curl -s localhost:12345/hostname ; echo
	killall -INT hinfosvc

clean:
	rm -f hinfosvc