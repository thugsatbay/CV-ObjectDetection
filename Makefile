all: CImg.h a3.cpp Classifier.h NearestNeighbor.h SVM.h overfeat.h eigen.h bag.h haar.h
	g++ -Wextra a3.cpp -o a3 -lX11 -lpthread -I. -Isiftpp -O3 siftpp/sift.cpp

clean:
	rm a3
