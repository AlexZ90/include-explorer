RAPIDJSON_DIR=/home/zalman/Downloads/git/rapidjson

all:
	mkdir -p bin
	g++ -o bin/include-explorer -std=c++11 src/*.cpp -I${RAPIDJSON_DIR}/include/

install:
	mkdir -p /opt/include-explorer
	cp bin/include-explorer /opt/include-explorer
	rm -f /usr/local/bin/include-explorer
	ln -s /opt/include-explorer/include-explorer /usr/local/bin/include-explorer

.PHOHY: prepare_test
prepare_test:
	cd test && bear make