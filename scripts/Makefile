#../bin/qc:./main.cpp ./*.h
#	g++ -g -O3 --std=c++11 ./main.cpp -Wall -lboost_filesystem -lboost_system -lz -o ../bin/qc
#
all : ../bin/test ../bin/qc

../bin/qc:./qc.cpp ./GuessEncoding.h ./HalvadeFiles.h ./QualityCheck.h
	g++ -O3 --std=c++11 ./qc.cpp -Wall -L ${HOME}/usr/lib -lboost_iostreams -o ../bin/qc

../bin/test:./testBoost.cpp
	g++ -g ./testBoost.cpp --std=c++11 -L ${HOME}/usr/lib -lboost_iostreams -o ../bin/test
#	g++ -g ./test.cpp --std=c++11 -Wall -o ../bin/test
