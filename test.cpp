#include "CImg.h"

using namespace cimg_library;
//using namespace std;

int main(){
	CImg<double> asd("train231/bagel/106970_231.jpg");
	asd.resize(231, 231, 1, 1);
	asd.get_normalize(0, 255).save("temp.jpg");
	return 0;
}

