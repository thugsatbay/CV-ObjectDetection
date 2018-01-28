class Haar : public Classifier{


	protected:
			vector< CImg<double> > picListTrain;
			vector< CImg<double> > picListTest;
			vector< vector<int> > allSift;
			vector < vector<SiftDescriptor> > categorySift;
			int smallestDimension;


public:

		Haar(const vector<string> &_class_list) : Classifier(_class_list) {
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


		//create integral representation of image where each value is
		//sum of all values to the left and above
		CImg<int> calcSumMap(CImg<double> pic) {
			CImg<int> output (pic.width(), pic.height(), 1, 1, 0);
			for(int y = 0; y<pic.height(); y++) {
				double total = 0;
				for(int x = 0; x<pic.width(); x++) {
					for(int yy = 0; yy<=y; yy++) {
						total += pic(x,yy);
					}
					output(x,y) = (int)total;
				}
			}
			return output;
		}

		//calculates the values under of one rectangle of filter
		//mapX and mapY represent the location of master loop in the sumMap
		int violaJonesOneRectangle(CImg<int>* sumMap, int mapX, int mapY, int multFactor, int filterX1, int filterY1, int filterX2, int filterY2) {
			filterX1 += mapX;
			filterX2 += mapX;
			filterY1 += mapY;
			filterY2 += mapY;
			
			int area1 = (*sumMap)(filterX1, filterY1);
			int area2 = (*sumMap)(filterX1, filterY2);
			int area3 = (*sumMap)(filterX2, filterY1);
			int area4 = (*sumMap)(filterX2, filterY2);
			int total = area4 - area2 - area3 + area1;
			total *= multFactor;
			return total;
		}


		//finds highest value for a haar filter by using integral map (sumMap)
		//filters contains one big positive filter and one or two negative child filters
		int violaJones(CImg<int>* sumMap, vector<int> filters) {
			int highest = -100000;
			for(int y = 0; y<sumMap->height() - filters[1]; y+=5) {
				for(int x = 0; x<sumMap->width() - filters[0]; x+=5) {
					int total = 0;
					for(unsigned int i = 2; i<filters.size(); i+=5) {
						total += violaJonesOneRectangle(sumMap, x, y, filters[i], filters[i+1], filters[i+2], filters[i+3], filters[i+4]);
					}
					if(total > highest) highest = total;
				}
			}
			return highest;
		}


		//haar filter is (width, height, box1multfactor, box1x1, box1y1, box1x2, box1y2, box2...)
		//each filter will have one big positive area and one or two negative child areas
		//max size is 24 and min size is 5
		vector< vector<int> > generateHaar() {
			vector< vector<int> > haarFilters;
			
			for(int i = 0; i<1000; i++) {
				int count = (rand() % 2) + 2;
				vector<int> tempFilter;
				int sizeX = (rand() % 25) + 10;
				int sizeY = (rand() % 25) + 10;
				for(int ii = 0; ii<count; ii++) {
					int mult = -2;
					int x1 = rand() % (sizeX-2);
					int y1 = rand() % (sizeY-2);
					int x2 = (rand() % (sizeX/2)) + x1;
					int y2 = (rand() % (sizeY/2)) + y1;

					if(x2 > sizeX) x2 = sizeX;
					if(y2 > sizeY) y2 = sizeY;
					if(ii == 0) {
						x1 = 0;
						y1 = 0;
						x2 = sizeX;
						y2 = sizeY;
						mult = 1;
					}
					tempFilter.push_back(mult);
					tempFilter.push_back(x1);
					tempFilter.push_back(y1);
					tempFilter.push_back(x2);
					tempFilter.push_back(y2);
				}
				vector<int> finalFilter;
				finalFilter.push_back(sizeX);
				finalFilter.push_back(sizeY);
				for(unsigned int ii = 0; ii<tempFilter.size(); ii++) {
					if(ii % 5 == 0) finalFilter.push_back(tempFilter[ii]);
					else if(ii % 5 == 1 || ii % 5 == 3) finalFilter.push_back(tempFilter[ii]);
					else finalFilter.push_back(tempFilter[ii]);
				}
				haarFilters.push_back(finalFilter);
			}
			return haarFilters;
		}



		virtual void train(const Dataset &filenames){ 

			vector< vector<int> > haarFilters = generateHaar();
			
			getImagesTrain();
			
			//creates list of integral images
			vector< CImg<int> > sumList;
			for(unsigned int i = 0; i<picListTrain.size(); i++) {
				sumList.push_back(calcSumMap(picListTrain[i]));
			}
			
			vector< vector<int> > categoryAverages;
			vector< vector<int> > picScores;
			vector<int> totalAverages;
			
			//computes highest haar filter values for each image and pushes these to picScores
			//will then compute average total value for each filter and average value by category
			for(unsigned int i = 0; i<haarFilters.size(); i++) {
				cout << i << "/" << haarFilters.size() << "\n";
				int totalScore = 0;
				int categoryScore = 0;
				vector<int> categoryRow;
				vector<int> picRow;
				for(unsigned int ii = 0; ii<sumList.size(); ii++) {
					int score = violaJones(&sumList[ii], haarFilters[i]);
					totalScore += score;
					categoryScore += score;
					picRow.push_back(score);
					if(ii % 50 == 49) {
						categoryRow.push_back(categoryScore/50);
						categoryScore = 0;
					}
				}
				totalAverages.push_back(totalScore/sumList.size());
				categoryAverages.push_back(categoryRow);
				picScores.push_back(picRow);
			}
			
			vector< vector<int> > hashedDifferences;
			int currentChoice = 0;
			vector<int> alreadyChosen;
			for(int ii = 0; ii<25; ii++) alreadyChosen.push_back(0);
			
			//will compute highest differences between a filter's total average and each category average
			//alreadyChosen is a hash table used to balance this process so not just one category is being used over and over
			//hashedDifferences is another hash table holding the image indexes at locations based on the average difference
			for(unsigned int i = 0; i<totalAverages.size(); i++) {
				double biggestDiff = -1000000;
				for(unsigned int ii = 0; ii<categoryAverages[i].size(); ii++) {
					if(totalAverages[i] > -1 && totalAverages[i] < 1) totalAverages[i] = 1;
					double diff = abs((double)(totalAverages[i] - categoryAverages[i][ii])/(double)totalAverages[i]);
					if(diff > biggestDiff && alreadyChosen[ii] == 0) {
						biggestDiff = diff;
						currentChoice = ii;
					}
				}
				unsigned int diffIndex = biggestDiff*10;
				while(hashedDifferences.size() < diffIndex+1) {
					vector<int> row;
					hashedDifferences.push_back(row);
				}
				hashedDifferences[diffIndex].push_back(i);
				alreadyChosen[currentChoice] = 1;
				if(i % 25 == 24) {
					for(unsigned int ii = 0; ii<alreadyChosen.size(); ii++) alreadyChosen[ii] = 0;
				}
			}
			
			
			//the highestDifference hash table is read from top to bottom to pull out best haar filters
			vector<int> bestHaar;
			int bestSize = 125;
			for(unsigned int i = hashedDifferences.size(); i>0 && bestSize >= 0; i--) {
				for(unsigned int ii = 0; ii<hashedDifferences[i-1].size() && bestSize >= 0; ii++) {
					bestSize--;
					bestHaar.push_back(hashedDifferences[i-1][ii]);
				}
			}
		  	
		  	//following writes to file the image scores for the best haar filters and the best haar filters themselves
		  	string finalScores = "";
			for(unsigned int i = 0; i<bestHaar.size(); i++) {
				for(unsigned int ii = 0; ii<picScores[bestHaar[i]].size()-1; ii++) {
					finalScores += toString(picScores[bestHaar[i]][ii]) + "\t";
				}
				finalScores += toString(picScores[bestHaar[i]][picScores[bestHaar[i]].size()-1]) + "\n";
			}
			std::ofstream file4("svm-process/part2-haar-PicScores.txt");
		  	if (file4.is_open()) {
		    	file4 << finalScores;
		  	}
		  	

		  	string haarFilts = "";
		  	for(unsigned int i = 0; i<bestHaar.size(); i++) {
		  		for(unsigned int ii = 0; ii<haarFilters[bestHaar[i]].size()-1; ii++) {
		  			haarFilts += toString(haarFilters[bestHaar[i]][ii]) + "\t";
		  		}
		  		cout << "Processing and saving the Haar Filters : " << i << "/" << bestHaar.size() << "\n";
		  		haarFilts += toString(haarFilters[bestHaar[i]][haarFilters[bestHaar[i]].size()-1]) + "\n";
		  	}
			std::ofstream file2("svm-process/part2-haar-Filters.txt");
		  	if (file2.is_open()) {
		    	file2 << haarFilts;
		  	}

		  	std::ofstream file9("svm-process/part2-haar-svmText.txt");
		  	if (file9.is_open()) {
		  		for(unsigned int i = 0; i<picScores[0].size(); i++) {
		  			file9 << i/50+1 << " ";
		  			for(unsigned int ii = 0; ii<bestHaar.size(); ii++) {
		  				file9 << ii+1 << ":" << picScores[bestHaar[ii]][i] << " ";
		  			}
		  			file9 << endl;
		  		}
			}	
		  	int result = system("./svm_multiclass_learn -e 30.0 -c 0.001 svm-process/part2-haar-svmText.txt svm-process/part2-haar-svmModelHaar.txt");
		}





		virtual void load_model(const Dataset &filenames){
			
			//read from file data from training
			vector<string> scoreList = parseInput("svm-process/part2-haar-PicScores.txt");
			vector< vector<int> > categoryScores;
			for(unsigned int i = 0; i<scoreList.size(); i++) {
				vector<int> catRow = split(scoreList[i], '\t');
				categoryScores.push_back(catRow);
			}
			
			vector<string> filterList = parseInput("svm-process/part2-haar-Filters.txt");
			vector< vector<int> > haarFilters;
			for(unsigned int i = 0; i<filterList.size(); i++) {
				vector<int> filterRow = split(filterList[i], '\t');
				haarFilters.push_back(filterRow);
			}
			
			getImagesTest();
			
			
			vector< CImg<int> > sumList;
			for(unsigned int i = 0; i<picListTest.size(); i++) {
				sumList.push_back(calcSumMap(picListTest[i]));
			}
			
			
			//run violaJones on test images using filters from training
			vector< vector<int> > testScores;
			for(unsigned int i = 0; i<haarFilters.size(); i++) {
				cout << i << "/" << haarFilters.size() << "\n";
				vector<int> scoreRow;
				for(unsigned int ii = 0; ii<sumList.size(); ii++) {
					scoreRow.push_back(violaJones(&sumList[ii], haarFilters[i]));
				}
				testScores.push_back(scoreRow);
			}


			std::ofstream file9("svm-process/part2-haar-svmTextTest.txt");
		  	if (file9.is_open()) {
		  		for(unsigned int i = 0; i<testScores[0].size(); i++) {
		  			file9 << i/10+1 << " ";
		  			for(unsigned int ii = 0; ii<testScores.size(); ii++) {
		  				file9 << ii+1 << ":" << testScores[ii][i] << " ";
		  			}
		  			file9 << endl;
		  		}
			}	
		  	int result = system("./svm_multiclass_classify svm-process/part2-haar-svmTextTest.txt svm-process/part2-haar-svmModelHaar.txt svm-process/part2-haar-svmPredictionsHaar.txt");
			
			
			//calculate best match in training set using euclidian distance
			//matching done based on fifty training images per category and ten test images per category
			// int numMatched = 0;
			// for(unsigned int i = 0; i< testScores[0].size(); i++) {
			// 	int closestMatchIndex = -1;
			// 	long closestDist = 10000000;
			// 	for(unsigned int ii = 0; ii<categoryScores[0].size(); ii++) {
			// 		long dist = 0;
			// 		for(int iii = 0; iii<testScores.size(); iii++) dist += pow(categoryScores[iii][ii] - testScores[iii][i], 2);
			// 		dist = sqrt(dist);
			// 		if(dist < closestDist) {
			// 			closestDist = dist;
			// 			closestMatchIndex = ii;
			// 		}
			// 	}
			// 	if(i/10 == closestMatchIndex/50) numMatched++;
			// }
			// cout << numMatched << "/" << testScores[0].size() << "\n";
		}
			virtual string classify(const string &filename){

			ifstream open_predict_dat;
						open_predict_dat.open("svm-process/part2-haar-svmPredictionsHaar.txt");
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








