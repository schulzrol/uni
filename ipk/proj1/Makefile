
all: hinfosvc.cpp
	g++ hinfosvc.cpp -ggdb -o hinfosvc

test: all
	killall -q hinfosvc ; true
	./hinfosvc 12345 &
	sleep 5
	curl localhost:12345/load ; echo
	curl localhost:12345/cpu-name ; echo
	curl localhost:12345/hostname ; echo
	killall hinfosvc
