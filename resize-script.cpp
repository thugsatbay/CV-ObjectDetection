#include "CImg.h"
#include <ctime>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <sys/types.h>
#include <dirent.h>
#include <map>
#include <numeric>

using namespace cimg_library;
using namespace std;

int main(){
	string directory("train");
	string output_directory("train231");
	DIR *dir = opendir(directory.c_str());
	DIR *sub_dir;
  	if(!dir)
    	throw std::string("Can't find directory " + directory);
  
  	struct dirent *dirent, *sub_dirent;
  	while ((dirent = readdir(dir))) 
    	if(dirent->d_name[0] != '.'){
			sub_dir = opendir((directory + "/" + dirent->d_name).c_str());
			while ((sub_dirent = readdir(sub_dir)))
				if(sub_dirent->d_name[0] != '.'){
					string file_name(directory + "/" + dirent->d_name + "/" + sub_dirent->d_name);
					string out_file_name(output_directory + "/" + dirent->d_name + "/" + sub_dirent->d_name);
					cout << file_name.c_str() << endl;
					CImg<double> asd(file_name.c_str());
					asd.resize(231, 231);
					asd.get_normalize(0, 255).save(out_file_name.c_str());
				}
		}
	return 0;
}

