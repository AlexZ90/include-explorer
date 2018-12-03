RAPIDJSON_DIR=/home/alex/Downloads/git/rapidjson

all:
	g++ -o include-explorer -std=c++11 main.cpp -I${RAPIDJSON_DIR}/include/

.PHOHY: prepare_test
prepare_test:
	cd test && bear make