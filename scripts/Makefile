#../bin/qc:./main.cpp ./*.h
#	g++ -g -O3 --std=c++11 ./main.cpp -Wall -lboost_filesystem -lboost_system -lz -o ../bin/qc
#
all : ../bin/test

../bin/qc : ./qc.cpp ./DecompressedFiles.h ./QualityCheck.h
	g++ -O3 --std=c++11 ./qc.cpp -Wall -o ../bin/qc

#../bin/test:./testMat.cpp ./QualityCheck.h ./Matrix.h
#	g++ --std=c++11 ./testMat.cpp -Wall -o ../bin/test
../bin/test : ./test.cpp ./DecompressedFiles.h
	g++ -O3 -g --std=c++11 ./test.cpp -Wall -o ../bin/test
