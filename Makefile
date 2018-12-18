RAPIDJSON_DIR=/home/alex/Downloads/git/rapidjson

all:
	g++ -o include-explorer -std=c++11 main.cpp -I${RAPIDJSON_DIR}/include/

install:
	mkdir -p /opt/include-explorer
	cp include-explorer /opt/include-explorer
	ln -s /opt/include-explorer/include-explorer /usr/local/bin/include-explorer

.PHOHY: prepare_test
prepare_test:
	cd test && bear make