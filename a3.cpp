// B657 assignment 3 skeleton code, D. Crandall
//
// Compile with: "make"
//
// This skeleton code implements nearest-neighbor classification
// using just matching on raw pixel values, but on subsampled "tiny images" of
// e.g. 20x20 pixels.
//
// It defines an abstract Classifier class, so that all you have to do
// :) to write a new algorithm is to derive a new class from
// Classifier containing your algorithm-specific code
// (i.e. load_model(), train(), and classify() methods) -- see
// NearestNeighbor.h for a prototype.  So in theory, you really
// shouldn't have to modify the code below or the code in Classifier.h
// at all, besides adding an #include and updating the "if" statement
// that checks "algo" below.
//
// See assignment handout for command line and project specifications.
//
#include "CImg.h"
#ifdef Success
  #undef Success
#endif
#include <ctime>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <Sift.h>
#include <sys/types.h>
#include <dirent.h>
#include <map>
#include <numeric>
#include "Eigen/Eigenvalues"
#include <sstream>

//Use the cimg namespace to access the functions easily
using namespace cimg_library;
using namespace std;
using Eigen::MatrixXd;
using Eigen::EigenSolver;

// Dataset data structure, set up so that e.g. dataset["bagel"][3] is
// filename of 4th bagel image in the dataset
typedef map<string, vector<string> > Dataset;

string toString(int num)
{
  string sign("");
  if (num == 0){
    return "0";
  }
  if (num < 0){
    num = num * -1;
    sign = "-";
  }
	int rem, len = 0, n;
    n = num;
    while (n != 0)
    {
        len++;
        n /= 10;
    }
    char *value = new char[len + 1];
    for (int i = 0; i < len; i++)
    {
        rem = num % 10;
        num = num / 10;
        value[len - i - 1] = rem + '0';
    }
    value[len] = '\0';
    string result(value);
	return (sign + result);
}


#include <Classifier.h>
#include <NearestNeighbor.h>
#include <SVM.h>
#include <overfeat.h>
#include <eigen.h>
#include <bag.h>
#include <haar.h>
// Figure out a list of files in a given directory.
//
vector<string> files_in_directory(const string &directory, bool prepend_directory = false)
{
  vector<string> file_list;
  DIR *dir = opendir(directory.c_str());
  if(!dir)
    throw std::string("Can't find directory " + directory);
  
  struct dirent *dirent;
  while ((dirent = readdir(dir))) 
    if(dirent->d_name[0] != '.')
      file_list.push_back((prepend_directory?(directory+"/"):"")+dirent->d_name);

  closedir(dir);
  return file_list;
}






// //
// //
// //Steve Code
// //
// //



// std::vector<int> split(const std::string &s, char delim) {
//     std::vector<int> elems;
//     std::stringstream ss;
//     ss.str(s);
//     std::string item;
//     while (std::getline(ss, item, delim)) {
//         elems.push_back(atoi(item.c_str()));
//     }
//     return elems;
// }


// vector<string> parseInput(string fName) {
//     std::ifstream inFile(fName.c_str());
//     string line;
//     vector<string> inList = vector<string>();
//     if(inFile.is_open()) {
//         while(getline(inFile, line)) inList.push_back(line);
//     }
//     inFile.close();
//     return inList;
// }



// vector< CImg<double> > picListTrain;
// vector< CImg<double> > picListTest;
// vector< vector<int> > allSift;
// vector < vector<SiftDescriptor> > categorySift;
// int smallestDimension = 73;


// //reads training images into picList
// //will convert to gray, crop, and shrink images to 73x73
// void getImagesTrain() {
//   if(picListTrain.size() > 0) return;
//   Dataset filenames; 
//     vector<string> class_list = files_in_directory("train");
//     for(vector<string>::const_iterator c = class_list.begin(); c != class_list.end(); ++c)
//       filenames[*c] = files_in_directory("train/" + *c, true);
      
//     map<string, vector<string> >::iterator it;
//   for ( it = filenames.begin(); it != filenames.end(); it++ ) {
//       std::cout << it->first << "\t";
//       vector<string> names = it->second;
//       for(unsigned int i = 0; i<names.size(); i++) {
//         cout << names[i] << " ";
//         CImg<double> pic(names[i].c_str());
//         CImg<double> gray = pic.get_RGBtoYCbCr().get_channel(0);
//         picListTrain.push_back(gray);
//         if(pic.width() < smallestDimension) smallestDimension = pic.width();
//         if(pic.height() < smallestDimension) smallestDimension = pic.height();
//       }
//   }
//   for(unsigned int i = 0; i<picListTrain.size(); i++) {
//     if(picListTrain[i].width() > picListTrain[i].height()) {
//       int diff = (picListTrain[i].width() - picListTrain[i].height())/2;
//       picListTrain[i] = picListTrain[i].get_crop(diff, 0, picListTrain[i].width()-diff, picListTrain[i].height());
//     }
//     else if(picListTrain[i].height() > picListTrain[i].width()) {
//       int diff = (picListTrain[i].height() - picListTrain[i].width())/2;
//       picListTrain[i] = picListTrain[i].get_crop(0, diff, picListTrain[i].width(), picListTrain[i].height()-diff);
//     }
//     picListTrain[i] = picListTrain[i].resize(smallestDimension, smallestDimension);
//   }
// }

// //reads testing images into picList
// //will convert to gray, crop, and shrink images to 73x73
// void getImagesTest() {
//   if(picListTest.size() > 0) return;
//   Dataset filenames; 
//     vector<string> class_list = files_in_directory("test");
//     for(vector<string>::const_iterator c = class_list.begin(); c != class_list.end(); ++c)
//       filenames[*c] = files_in_directory("test/" + *c, true);
      
//     map<string, vector<string> >::iterator it;
//   for ( it = filenames.begin(); it != filenames.end(); it++ ) {
//       std::cout << it->first << "\t";
//       vector<string> names = it->second;
//       for(unsigned int i = 0; i<names.size(); i++) {
//         cout << names[i] << " ";
//         CImg<double> pic(names[i].c_str());
//         CImg<double> gray = pic.get_RGBtoYCbCr().get_channel(0);
//         picListTest.push_back(gray);
//         if(pic.width() < smallestDimension) smallestDimension = pic.width();
//         if(pic.height() < smallestDimension) smallestDimension = pic.height();
//       }
//   }
//   for(unsigned int i = 0; i<picListTest.size(); i++) {
//     if(picListTest[i].width() > picListTest[i].height()) {
//       int diff = (picListTest[i].width() - picListTest[i].height())/2;
//       picListTest[i] = picListTest[i].get_crop(diff, 0, picListTest[i].width()-diff, picListTest[i].height());
//     }
//     else if(picListTest[i].height() > picListTest[i].width()) {
//       int diff = (picListTest[i].height() - picListTest[i].width())/2;
//       picListTest[i] = picListTest[i].get_crop(0, diff, picListTest[i].width(), picListTest[i].height()-diff);
//     }
//     picListTest[i] = picListTest[i].resize(smallestDimension, smallestDimension);
//   }
// }


// //reads training images into picList
// //will convert to gray, crop, and shrink images to 73x73
// void getImagesTrainEigen() {
//   if(picListTrain.size() > 0) return;
//   Dataset filenames; 
//     vector<string> class_list = files_in_directory("train");
//     for(vector<string>::const_iterator c = class_list.begin(); c != class_list.end(); ++c)
//       filenames[*c] = files_in_directory("train/" + *c, true);
      
//     map<string, vector<string> >::iterator it;
//   for ( it = filenames.begin(); it != filenames.end(); it++ ) {
//       std::cout << it->first << "\t";
//       vector<string> names = it->second;
//       for(unsigned int i = 0; i<names.size(); i++) {
//         cout << names[i] << " ";
//         CImg<double> pic(names[i].c_str());
//         //CImg<double> gray = pic.get_RGBtoYCbCr().get_channel(0);
//         CImg<double> gray = pic.get_RGBtoHSI().get_channel(2);
//         picListTrain.push_back(gray);
//         if(pic.width() < smallestDimension) smallestDimension = pic.width();
//         if(pic.height() < smallestDimension) smallestDimension = pic.height();
//       }
//   }
//   for(unsigned int i = 0; i<picListTrain.size(); i++) {
//     if(picListTrain[i].width() > picListTrain[i].height()) {
//       int diff = (picListTrain[i].width() - picListTrain[i].height())/2;
//       picListTrain[i] = picListTrain[i].get_crop(diff, 0, picListTrain[i].width()-diff, picListTrain[i].height());
//     }
//     else if(picListTrain[i].height() > picListTrain[i].width()) {
//       int diff = (picListTrain[i].height() - picListTrain[i].width())/2;
//       picListTrain[i] = picListTrain[i].get_crop(0, diff, picListTrain[i].width(), picListTrain[i].height()-diff);
//     }
//     picListTrain[i] = picListTrain[i].resize(smallestDimension, smallestDimension);
//   }
// }

// //reads testing images into picList
// //will convert to gray, crop, and shrink images to 73x73
// void getImagesTestEigen() {
//   if(picListTest.size() > 0) return;
//   Dataset filenames; 
//     vector<string> class_list = files_in_directory("test");
//     for(vector<string>::const_iterator c = class_list.begin(); c != class_list.end(); ++c)
//       filenames[*c] = files_in_directory("test/" + *c, true);
      
//     map<string, vector<string> >::iterator it;
//   for ( it = filenames.begin(); it != filenames.end(); it++ ) {
//       std::cout << it->first << "\t";
//       vector<string> names = it->second;
//       for(unsigned int i = 0; i<names.size(); i++) {
//         cout << names[i] << " ";
//         CImg<double> pic(names[i].c_str());
//         //CImg<double> gray = pic.get_RGBtoYCbCr().get_channel(0);
//         CImg<double> gray = pic.get_RGBtoHSI().get_channel(2);
//         picListTest.push_back(gray);
//         if(pic.width() < smallestDimension) smallestDimension = pic.width();
//         if(pic.height() < smallestDimension) smallestDimension = pic.height();
//       }
//   }
//   for(unsigned int i = 0; i<picListTest.size(); i++) {
//     if(picListTest[i].width() > picListTest[i].height()) {
//       int diff = (picListTest[i].width() - picListTest[i].height())/2;
//       picListTest[i] = picListTest[i].get_crop(diff, 0, picListTest[i].width()-diff, picListTest[i].height());
//     }
//     else if(picListTest[i].height() > picListTest[i].width()) {
//       int diff = (picListTest[i].height() - picListTest[i].width())/2;
//       picListTest[i] = picListTest[i].get_crop(0, diff, picListTest[i].width(), picListTest[i].height()-diff);
//     }
//     picListTest[i] = picListTest[i].resize(smallestDimension, smallestDimension);
//   }
// }






//
//
//Steve Code Ends
//
//



int main(int argc, char **argv)
{
  try {
    if(argc < 3)
      throw string("Insufficent number of arguments");

    string mode = argv[1];
    string algo = argv[2];

    // Scan through the "train" or "test" directory (depending on the
    //  mode) and builds a data structure of the image filenames for each class.
    Dataset filenames; 
    vector<string> class_list = files_in_directory(mode);
    for(vector<string>::const_iterator c = class_list.begin(); c != class_list.end(); ++c)
      filenames[*c] = files_in_directory(mode + "/" + *c, true);

    // set up the classifier based on the requested algo
    Classifier *classifier=0;
    if(algo == "nn")
      classifier = new NearestNeighbor(class_list);
    else if(algo == "baseline")
      classifier = new SVM(class_list);
    else if(algo == "deep")
      classifier = new Overfeat(class_list);
    else if(algo == "eigen")
      classifier = new EigenFood(class_list);
    else if(algo == "bow")
      classifier = new Bag(class_list);
    else if(algo == "haar")
      classifier = new Haar(class_list);
    else
      throw std::string("unknown classifier " + algo);

    // now train or test!
    if(mode == "train")
      classifier->train(filenames);
    else if(mode == "test")
      classifier->test(filenames);
    else
      throw std::string("unknown mode!");
  }
  catch(const string &err) {
    cerr << "Error: " << err << endl;
  }
}