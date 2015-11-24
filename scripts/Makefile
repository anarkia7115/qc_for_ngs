#../bin/qc:./main.cpp ./*.h
#	g++ -g -O3 --std=c++11 ./main.cpp -Wall -lboost_filesystem -lboost_system -lz -o ../bin/qc
#
all : ../bin/test
../bin/qc:./qc.cpp ./DecompressedFiles.h ./QualityCheck.h
	g++ -O3 --std=c++11 ./qc.cpp -Wall -o ../bin/qc

../bin/test:./test.cpp ./DecompressedFiles.h
	g++ -g ./test.cpp --std=c++11 -Wall -o ../bin/test
