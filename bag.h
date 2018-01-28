class Bag : public Classifier{


	protected:
			vector< CImg<double> > picListTrain;
			vector< CImg<double> > picListTest;
			vector< vector<int> > allSift;
			vector < vector<SiftDescriptor> > categorySift;
			int smallestDimension;		



public:

		Bag(const vector<string> &_class_list) : Classifier(_class_list) {
			smallestDimension = 73;
		}


				std::vector<int> split(const std::string &s, char delim) {
				    std::vector<int> elems;
				    std::stringstream ss;
				    ss.str(s);
				    std::string item;
				    while (std::getline(ss, item, delim)) {
				        elems.push_back(atoi(item.c_str()));
				    }
				    return elems;
				}


				vector<string> parseInput(string fName) {
				    std::ifstream inFile(fName.c_str());
				    string line;
				    vector<string> inList = vector<string>();
				    if(inFile.is_open()) {
				        while(getline(inFile, line)) inList.push_back(line);
				    }
				    inFile.close();
				    return inList;
				}



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


			void getImagesTrain() {
			  if(picListTrain.size() > 0) return;
			  Dataset filenames; 
			    vector<string> class_list = files_in_directory("train");
			    for(vector<string>::const_iterator c = class_list.begin(); c != class_list.end(); ++c)
			      filenames[*c] = files_in_directory("train/" + *c, true);
			      
			    map<string, vector<string> >::iterator it;
			  for ( it = filenames.begin(); it != filenames.end(); it++ ) {
			      std::cout << it->first << endl;
			      vector<string> names = it->second;
			      for(unsigned int i = 0; i<names.size(); i++) {
			        cout << "Reading Image : " << names[i] << endl;
			        CImg<double> pic(names[i].c_str());
			        CImg<double> gray = pic.get_RGBtoYCbCr().get_channel(0);
			        picListTrain.push_back(gray);
			        if(pic.width() < smallestDimension) smallestDimension = pic.width();
			        if(pic.height() < smallestDimension) smallestDimension = pic.height();
			      }
			  }
			  for(unsigned int i = 0; i<picListTrain.size(); i++) {
			    if(picListTrain[i].width() > picListTrain[i].height()) {
			      int diff = (picListTrain[i].width() - picListTrain[i].height())/2;
			      picListTrain[i] = picListTrain[i].get_crop(diff, 0, picListTrain[i].width()-diff, picListTrain[i].height());
			    }
			    else if(picListTrain[i].height() > picListTrain[i].width()) {
			      int diff = (picListTrain[i].height() - picListTrain[i].width())/2;
			      picListTrain[i] = picListTrain[i].get_crop(0, diff, picListTrain[i].width(), picListTrain[i].height()-diff);
			    }
			    picListTrain[i] = picListTrain[i].resize(smallestDimension, smallestDimension);
			  }
			}

			//reads testing images into picList
			//will convert to gray, crop, and shrink images to 73x73
			void getImagesTest() {
			  if(picListTest.size() > 0) return;
			  Dataset filenames; 
			    vector<string> class_list = files_in_directory("test");
			    for(vector<string>::const_iterator c = class_list.begin(); c != class_list.end(); ++c)
			      filenames[*c] = files_in_directory("test/" + *c, true);
			      
			    map<string, vector<string> >::iterator it;
			  int class_target = 1;
			  int index_file = 1;
			  for ( it = filenames.begin(); it != filenames.end(); it++ ) {
			      std::cout << it->first << endl;
			      image_class_number[toString(class_target)] = it->first;
			      vector<string> names = it->second;
			      for(unsigned int i = 0; i<names.size(); i++) {
			        test_file_number[names[i]] = index_file;
			        cout << "Reading Image : " << names[i] << " Class : " << class_target << " Index : " << index_file << endl;
			        index_file++;
			        CImg<double> pic(names[i].c_str());
			        CImg<double> gray = pic.get_RGBtoYCbCr().get_channel(0);
			        picListTest.push_back(gray);
			        if(pic.width() < smallestDimension) smallestDimension = pic.width();
			        if(pic.height() < smallestDimension) smallestDimension = pic.height();
			      }
			      class_target++;
			  }
			  for(unsigned int i = 0; i<picListTest.size(); i++) {
			    if(picListTest[i].width() > picListTest[i].height()) {
			      int diff = (picListTest[i].width() - picListTest[i].height())/2;
			      picListTest[i] = picListTest[i].get_crop(diff, 0, picListTest[i].width()-diff, picListTest[i].height());
			    }
			    else if(picListTest[i].height() > picListTest[i].width()) {
			      int diff = (picListTest[i].height() - picListTest[i].width())/2;
			      picListTest[i] = picListTest[i].get_crop(0, diff, picListTest[i].width(), picListTest[i].height()-diff);
			    }
			    picListTest[i] = picListTest[i].resize(smallestDimension, smallestDimension);
			  }
			}



		double calcDistance(vector<int> s1, vector<int> s2) {
			long total = 0;
			for(int l=0; l<128; l++) total += pow(s1[l] - s2[l], 2);
			return (int)sqrt(total);
		} 

		vector< vector<int> > kMeansSiftOnePass(vector< vector<int> > centroids) {
			int numClusters = 250;
			
			vector< vector<int> > membershipHashTable;
			for(int i = 0; i<numClusters; i++) {
				vector<int> row;
				membershipHashTable.push_back(row);
			}
			
			for(unsigned int i = 0; i<allSift.size(); i++) {
				double closestDist = 1000000;
				int closestIndex = -1;
				for(unsigned int ii = 0; ii<centroids.size(); ii++) {
					double dist = calcDistance(centroids[ii], allSift[i]);
					if(dist < closestDist) {
						closestDist = dist;
						closestIndex = ii;
					}
				}
				membershipHashTable[closestIndex].push_back(i);
			}
			
			vector< vector<int> > newCentroids;
			for(unsigned int i = 0; i<membershipHashTable.size(); i++) {
				vector<int> newCentroid (128, 0);
				for(unsigned int ii = 0; ii<membershipHashTable[i].size(); ii++) {
					for(int iii = 0; iii<128; iii++) newCentroid[iii] += allSift[membershipHashTable[i][ii]][iii];
				}
				for(int ii = 0; ii<128; ii++) newCentroid[ii] = newCentroid[ii] / membershipHashTable[i].size();
				newCentroids.push_back(newCentroid);
			}
			return newCentroids;
		}



		virtual void train(const Dataset &filenames){ 
			
			getImagesTrain();
			
			for(unsigned int i = 0; i<picListTrain.size(); i++) {
				cout << "On Image Number : " << (i+1) << " for processing" << "\n";
				vector<SiftDescriptor> descriptors = Sift::compute_sift(picListTrain[i]);
				categorySift.push_back(descriptors);
				for(unsigned int ii = 0; ii<descriptors.size(); ii++) {
					vector<int> row;
					for(int iii = 0; iii<128; iii++) row.push_back(descriptors[ii].descriptor[iii]);
					allSift.push_back(row);
				}
			}
			
			vector< vector<int> > picScores;
			for(int i = 0; i<1250; i++) {
				vector<int> row (250, 0);
				picScores.push_back(row);
			}
			
			vector<string> centroidText = parseInput("svm-process/part2-centroids.txt");
			vector< vector<int> > newCentroids;
			for(unsigned int i = 0; i<centroidText.size(); i++) {
				vector<int> row = split(centroidText[i], '\t');
				newCentroids.push_back(row);
			}
			
			int threshold = 330;
			for(unsigned int i = 0; i<categorySift.size(); i++) {
				for(unsigned int ii = 0; ii<categorySift[i].size(); ii++) {
					for(unsigned int iii = 0; iii<newCentroids.size(); iii++) {
						long total = 0;
						for(int iiii=0; iiii<128; iiii++) total += pow(categorySift[i][ii].descriptor[iiii] - newCentroids[iii][iiii], 2);
						total = sqrt(total);
						if(total < threshold) picScores[i][iii] += 1;
					}
				}
			}
			
		  	
		  	//following writes to file the centroids and picScores
		  	string finalScores = "";
			for(unsigned int i = 0; i<picScores.size(); i++) {
				for(unsigned int ii = 0; ii<picScores[i].size()-1; ii++) {
					finalScores += toString(picScores[i][ii]) + "\t";
				}
				finalScores += toString(picScores[i][picScores[i].size()-1]) + "\n";
			}
			std::ofstream file4("svm-process/part2-wordsPicScores.txt");
		  	if (file4.is_open()) {
		    	file4 << finalScores;
		  	}

		  	std::ofstream file9("svm-process/part2-bag-svmText.txt");
		  	if (file9.is_open()) {
		  		for(unsigned int i = 0; i<picScores.size(); i++) {
		  			file9 << i/50+1 << " ";
		  			for(unsigned int ii = 0; ii<picScores[i].size(); ii++) {
		  				file9 << ii+1 << ":" << picScores[i][ii] << " ";
		  			}
		  			file9 << endl;
		  		}
			}	
		  	int result = system("./svm_multiclass_learn -c 1 svm-process/part2-bag-svmText.txt svm-process/part2-bag-svmModelWords.txt");
		  	
		  	string centroidsText = "";
		  	for(unsigned int i = 0; i<newCentroids.size(); i++) {
		  		for(unsigned int ii = 0; ii<newCentroids[i].size()-1; ii++) {
		  			centroidsText += toString(newCentroids[i][ii]) + "\t";
		  		}
		  		centroidsText += toString(newCentroids[i][newCentroids[i].size()-1]) + "\n";
		  	}
			std::ofstream file2("svm-process/part2-centroids.txt");
		  	if (file2.is_open()) {
		    	file2 << centroidsText;
		  	}

		}




		void runWordsFullTrain() {
			
			getImagesTrain();
			
			for(unsigned int i = 0; i<picListTrain.size(); i++) {
				cout << i << "\n";
				vector<SiftDescriptor> descriptors = Sift::compute_sift(picListTrain[i]);
				categorySift.push_back(descriptors);
				for(unsigned int ii = 0; ii<descriptors.size(); ii++) {
					vector<int> row;
					for(int iii = 0; iii<128; iii++) row.push_back(descriptors[ii].descriptor[iii]);
					allSift.push_back(row);
				}
			}
			
			int numClusters = 250;
			int stride = allSift.size() / numClusters - 1;
			vector< vector<int> > centroids;
			for(int i = 0; i<numClusters; i++) centroids.push_back(allSift[i*stride]);
			
			vector< vector<int> > newCentroids = kMeansSiftOnePass(centroids);
			for(int i = 0; i<4; i++) {
				cout << i << "\n";
				newCentroids = kMeansSiftOnePass(newCentroids);
			}
			
			vector< vector<int> > picScores;
			for(int i = 0; i<1250; i++) {
				vector<int> row (250, 0);
				picScores.push_back(row);
			}
			
			int threshold = 330;
			for(unsigned int i = 0; i<categorySift.size(); i++) {
				for(unsigned int ii = 0; ii<categorySift[i].size(); ii++) {
					for(unsigned int iii = 0; iii<newCentroids.size(); iii++) {
						long total = 0;
						for(int iiii=0; iiii<128; iiii++) total += pow(categorySift[i][ii].descriptor[iiii] - newCentroids[iii][iiii], 2);
						total = sqrt(total);
						if(total < threshold) picScores[i][iii] += 1;
					}
				}
			}
			
		  	
		  	//following writes to file the centroids and picScores
		  	string finalScores = "";
			for(unsigned int i = 0; i<picScores.size(); i++) {
				for(unsigned int ii = 0; ii<picScores[i].size()-1; ii++) {
					finalScores += toString(picScores[i][ii]) + "\t";
				}
				finalScores += toString(picScores[i][picScores[i].size()-1]) + "\n";
			}
			std::ofstream file4("svm-process/part2-wordsPicScores.txt");
		  	if (file4.is_open()) {
		    	file4 << finalScores;
		  	}

		  	std::ofstream file9("svm-process/part2-bag-svmText.txt");
		  	if (file9.is_open()) {
		  		for(unsigned int i = 0; i<picScores.size(); i++) {
		  			file9 << i/50+1 << " ";
		  			for(unsigned int ii = 0; ii<picScores[i].size(); ii++) {
		  				file9 << ii+1 << ":" << picScores[i][ii] << " ";
		  			}
		  			file9 << endl;
		  		}
			}	
		  	int result = system("./svm_multiclass_learn -c 1 svm-process/part2-bag-svmText.txt svm-process/part2-bag-svmModelWords.txt");
		  	
		  	
		  	string centroidsText = "";
		  	for(unsigned int i = 0; i<newCentroids.size(); i++) {
		  		for(unsigned int ii = 0; ii<newCentroids[i].size()-1; ii++) {
		  			centroidsText += toString(newCentroids[i][ii]) + "\t";
		  		}
		  		centroidsText += toString(newCentroids[i][newCentroids[i].size()-1]) + "\n";
		  	}
			std::ofstream file2("svm-process/part2-centroids.txt");
		  	if (file2.is_open()) {
		    	file2 << centroidsText;
		  	}

		}





		virtual void load_model(const Dataset &filenames){
			
			//read from file data from training
			vector<string> scoreText = parseInput("svm-process/part2-wordsPicScores.txt");
			vector< vector<int> > trainingScores;
			for(unsigned int i = 0; i<scoreText.size(); i++) {
				vector<int> row = split(scoreText[i], '\t');
				trainingScores.push_back(row);
			}
			
			vector<string> centroidText = parseInput("svm-process/part2-centroids.txt");
			vector< vector<int> > newCentroids;
			for(unsigned int i = 0; i<centroidText.size(); i++) {
				vector<int> row = split(centroidText[i], '\t');
				newCentroids.push_back(row);
			}
			
			getImagesTest();
			
			for(unsigned int i = 0; i<picListTest.size(); i++) {
				cout << "On Image Number : " << (i+1) << " for processing" << "\n";
				vector<SiftDescriptor> descriptors = Sift::compute_sift(picListTest[i]);
				categorySift.push_back(descriptors);
			}
			
			vector< vector<int> > testScores;
			for(unsigned int i = 0; i<categorySift.size(); i++) {
				vector<int> row (250, 0);
				testScores.push_back(row);
			}
			
			int threshold = 330;
			for(unsigned int i = 0; i<categorySift.size(); i++) {
				for(unsigned int ii = 0; ii<categorySift[i].size(); ii++) {
					for(unsigned int iii = 0; iii<newCentroids.size(); iii++) {
						long total = 0;
						for(int iiii=0; iiii<128; iiii++) total += pow(categorySift[i][ii].descriptor[iiii] - newCentroids[iii][iiii], 2);
						total = sqrt(total);
						if(total < threshold) testScores[i][iii] += 1;
					}
				}
			}


			std::ofstream file9("svm-process/part2-bag-svmTextTest.txt");
		  	if (file9.is_open()) {
		  		for(unsigned int i = 0; i<testScores.size(); i++) {
		  			int classNum = i/10 + 1;
		  			file9 << classNum << " ";
		  			for(unsigned int ii = 0; ii<testScores[i].size(); ii++) {
		  				file9 << ii+1 << ":" << testScores[i][ii] << " ";
		  			}
		  			file9 << endl;
		  		}
		  	}
		  	int result = system("./svm_multiclass_classify svm-process/part2-bag-svmTextTest.txt svm-process/part2-bag-svmModelWords.txt svm-process/part2-bag-svmPredictionWords.txt");
			

			
			
			//calculate best match in training set using euclidian distance
			//matching done based on fifty training images per category and ten test images per category
			int numMatched = 0;
			for(unsigned int i = 0; i< testScores.size(); i++) {
				unsigned int closestMatchIndex = 0;
				long closestDist = 10000000;
				for(unsigned int ii = 0; ii<trainingScores.size(); ii++) {
					long dist = 0;
					for(unsigned int iii = 0; iii<testScores[0].size(); iii++) dist += pow(trainingScores[ii][iii] - testScores[i][iii], 2);
					dist = sqrt(dist);
					if(dist < closestDist) {
						closestDist = dist;
						closestMatchIndex = ii;
					}
				}
				//Made unsigned int bcz of comparison warning
				if(i/10 == closestMatchIndex/50) numMatched++;
			}
			cout << numMatched << "/" << testScores[0].size() << "\n";
		}



		virtual string classify(const string &filename){

			ifstream open_predict_dat;
						open_predict_dat.open("svm-process/part2-bag-svmPredictionWords.txt");
						int file_target = test_file_number[filename];
						string line;
						int count_line = 0;
						int class_target = 0;
						if ( open_predict_dat.is_open()){
							while ( getline (open_predict_dat,line) ){
									++count_line;
									if (count_line == file_target){
										string segment;
										stringstream sss;
										sss.str(line);
										getline(sss, segment, ' ');
										open_predict_dat.close();
										cout << image_class_number[segment] << endl;
										return image_class_number[segment];
									}
							}
						}
						open_predict_dat.close();
						cout << "Returned NULL Some Issue" << endl;
						return "";
		}

};











