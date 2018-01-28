
class EigenFood : public Classifier {
		
	protected:
			vector< CImg<double> > picListTrain;
			vector< CImg<double> > picListTest;
			vector< vector<int> > allSift;
			vector < vector<SiftDescriptor> > categorySift;
			int smallestDimension;		

	public:

			EigenFood(const vector<string> &_class_list) : Classifier(_class_list) {
				smallestDimension = 73;
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



			virtual void load_model(const Dataset &filenames){

			MatrixXd eigenMatrix(5329,250);
			std::ifstream file("svm-process/part2-eigen.txt");
			if (file.is_open()) {
				for(int y = 0; y<5329; y++) {
					for(int x = 0; x<250; x++) {
						file >> eigenMatrix(y,x);
					}
				}
			}
			
			MatrixXd trainMatrix(1250,250);
			std::ifstream file2("svm-process/part2-finalDims.txt");
			if (file2.is_open()) {
				for(int y = 0; y<1250; y++) {
					for(int x = 0; x<250; x++) {
						file2 >> trainMatrix(y,x);
					}
				}
			}
			
			getImagesTest();
			
			
			//unfolds images into a matrix where each is a column
			MatrixXd picMatrix(smallestDimension*smallestDimension, picListTest.size());
			for(unsigned int i = 0; i<picListTest.size(); i++) {
				int count = 0;
				for(int y = 0; y<picListTest[i].height(); y++) {
					for(int x = 0; x<picListTest[i].width(); x++) {
						picMatrix(count, i) = picListTest[i](x,y);
						count++;
					}
				}
			}
			
			//mean subtracted above matrix and multiplies against training eigenvectors
			MatrixXd centered = picMatrix.colwise() - picMatrix.rowwise().mean();
			MatrixXd finalMatrix = centered.transpose() * eigenMatrix;

									
									
			std::ofstream file9("svm-process/part2-eigen-svmTextTest.txt");
		  	if (file9.is_open()) {
		  		for(int i = 0; i<finalMatrix.rows(); i++) {
		  			int classNum = i/10 + 1;
		  			file9 << classNum << " ";
		  			for(int ii = 0; ii<finalMatrix.cols(); ii++) {
		  				file9 << ii+1 << ":" << finalMatrix(i,ii) << " ";
		  			}
		  			file9 << endl;
		  		}
		  	}
		  	int result = system("./svm_multiclass_classify svm-process/part2-eigen-svmTextTest.txt svm-process/part2-eigen-svmModelEigen.txt svm-process/part2-eigen-svmPredictionEigen.txt");
			

			
			//finds matches using euclidian distance between training matrix and above matrix
			//based on facts that there are ten test images per category and fifty training images
			/*int matchCount = 0;
			for(int i = 0; i<finalMatrix.rows(); i++) {
				double recordDistance = 1000000, recordIndex = 0;
				for(int ii = 0; ii<trainMatrix.rows(); ii++) {
					double distance = 0;
					for(int iii = 0; iii<trainMatrix.cols(); iii++) {
						distance += pow(trainMatrix(ii,iii) - finalMatrix(i,iii), 2);
					}
					distance = sqrt(distance);
					if(distance < recordDistance) {
						recordIndex = ii;
						recordDistance = distance;
					}
				}
				string matched = "False";
				if(i/10 == ((int)recordIndex)/50) {
					matched = "True";
					matchCount++;	
				}
				cout << "bestMatch\t" << i << "\t" << recordIndex << "\t" << matched << "\n";
			}
			cout << "MatchCount  " << matchCount << "/" << finalMatrix.rows() << "\n";*/
			
			
		}






		virtual void train(const Dataset &filenames){
			
			getImagesTrain();
			
			//create a matrix where each image is unfolded into a column
			MatrixXd picMatrix(smallestDimension*smallestDimension, picListTrain.size());
			for(unsigned int i = 0; i<picListTrain.size(); i++) {
				int count = 0;
				for(int y = 0; y<picListTrain[i].height(); y++) {
					for(int x = 0; x<picListTrain[i].width(); x++) {
						picMatrix(count, i) = picListTrain[i](x,y);
						count++;
					}
				}
			}
			
			//following will subtract the average pixel value at each x/y from each pixel at that x/y
			//then an abridged covariance matrix is computed and solved for eigenvectors
			MatrixXd centered = picMatrix.colwise() - picMatrix.rowwise().mean();
			MatrixXd covariance = (centered.adjoint() * centered) / double(picMatrix.cols() - 1);
			EigenSolver<MatrixXd> es(covariance);
			MatrixXd eigenValues = es.pseudoEigenvalueMatrix();
			MatrixXd eigenVectors = es.pseudoEigenvectors();
			
			//normalization
			MatrixXd normMatrix = picMatrix * eigenVectors;
			for(int i = 0; i<normMatrix.cols(); i++) {
				normMatrix.col(i) = normMatrix.col(i) * (1/sqrt(eigenValues(i,i)));
			}
			
			cout << "\n\npic   " << picMatrix.cols() << "\t" << picMatrix.rows() << "\n";
			cout << "\n\neigen " << eigenVectors.cols() << "\t" << eigenVectors.rows() << "\n";
			cout << "\n\nnorm  " << normMatrix.cols() << "\t" << normMatrix.rows() << "\n";
			
			
			//largest eigenvalues hashed out
			vector< vector<int> > hashedEigen;
			for(int i = 0; i<eigenValues.cols(); i++) {
				if(eigenValues(i,i) > 0) {
					unsigned int hash = eigenValues(i,i);
					while(hashedEigen.size() < hash+1) {
						vector<int> row;
						hashedEigen.push_back(row);
					}
					hashedEigen[hash].push_back(i);
				}
			}
			
			
			//hash table of eigenvalues scanned from top to bottom to pull out best eigenvectors
			MatrixXd eigenMatrix(smallestDimension*smallestDimension, 250);
			int count = 0;
			for(unsigned int i = hashedEigen.size()-1; i>0; i--) {
				for(unsigned int ii = 0; ii<hashedEigen[i].size(); ii++) {
					if(count < 250) eigenMatrix.col(count) = normMatrix.col(hashedEigen[i][ii]);
					count++;
					//cout << "eigenvalue: " << i << "\n";
				}
			}
			
			//final calculation takes mean subtracted values from a few steps back multiplied against the best eigenvectors
			MatrixXd finalMatrix = centered.transpose() * eigenMatrix;
			
			
			//saves eigenvectors and final matrix to file
			std::ofstream file4("svm-process/part2-eigen.txt");
		  	if (file4.is_open()) {
		    	file4 << eigenMatrix;
		  	}
		  	
			std::ofstream file5("svm-process/part2-finalDims.txt");
		  	if (file5.is_open()) {
		    	file5 << finalMatrix;
		  	}

		  	std::ofstream file9("svm-process/part2-eigen-svmText.txt");
		  	if (file9.is_open()) {
		  		for(int i = 0; i<finalMatrix.rows(); i++) {
		  			int classNum = i/50 + 1;
		  			file9 << classNum << " ";
		  			for(int ii = 0; ii<finalMatrix.cols(); ii++) {
		  				file9 << ii+1 << ":" << finalMatrix(i,ii) << " ";
		  			}
		  			file9 << endl;
		  		}
		  	}
		  	int result = system("./svm_multiclass_learn -c 5000 svm-process/part2-eigen-svmText.txt svm-process/part2-eigen-svmModelEigen.txt");
			
			
			//create images representing eigenvectors
			//not working as expected, unsure if an 'eigenface' is this or simply the mean subtracted vectors
			/*count = 0;
			double high = -10000, low = 10000, shiftAmount = 0, multFactor = 1;
			CImg<double> eigenPic(smallestDimension, smallestDimension);
			for(int i = 0; i<smallestDimension*smallestDimension; i++) {
				if(eigenMatrix(i,0) < low) low = eigenMatrix(i,0);
				else if (eigenMatrix(i,0) > high) high = eigenMatrix(i,0);
			}
			shiftAmount = low * -1;
			high = high + shiftAmount;
			if(high != 0) multFactor = 255/high;
			
			for(int y = 0; y<smallestDimension; y++) {
				for(int x = 0; x<smallestDimension; x++) {
					if(multFactor == 1) eigenPic(x,y) = eigenMatrix(count,0);
					else eigenPic(x,y) = (eigenMatrix(count,0) + shiftAmount)*multFactor;
					count++;
				}
			}
			string fn = "svm-process/eigenfood.png";
			eigenPic.save(fn.c_str());
			
			count = 0;
			high = -10000, low = 10000, shiftAmount = 0, multFactor = 1;
			CImg<double> eigenPic2(smallestDimension, smallestDimension);
			for(int i = 0; i<smallestDimension*smallestDimension; i++) {
				if(eigenMatrix(i,1) < low) low = eigenMatrix(i,1);
				else if (eigenMatrix(i,1) > high) high = eigenMatrix(i,1);
			}
			shiftAmount = low * -1;
			high = high + shiftAmount;
			if(high != 0) multFactor = 255/high;
			
			for(int y = 0; y<smallestDimension; y++) {
				for(int x = 0; x<smallestDimension; x++) {
					if(multFactor == 1) eigenPic2(x,y) = eigenMatrix(count,1);
					else eigenPic2(x,y) = (eigenMatrix(count,1) + shiftAmount)*multFactor;
					count++;
				}
			}
			string fn2 = "svm-process/eigenfood2.png";
			eigenPic2.save(fn2.c_str());
			
			count = 0;
			high = -10000, low = 10000, shiftAmount = 0, multFactor = 1;
			CImg<double> eigenPic3(smallestDimension, smallestDimension);
			for(int i = 0; i<smallestDimension*smallestDimension; i++) {
				if(eigenMatrix(i,2) < low) low = eigenMatrix(i,2);
				else if (eigenMatrix(i,2) > high) high = eigenMatrix(i,2);
			}
			shiftAmount = low * -1;
			high = high + shiftAmount;
			if(high != 0) multFactor = 255/high;
			
			for(int y = 0; y<smallestDimension; y++) {
				for(int x = 0; x<smallestDimension; x++) {
					if(multFactor == 1) eigenPic3(x,y) = eigenMatrix(count,2);
					else eigenPic3(x,y) = (eigenMatrix(count,2) + shiftAmount)*multFactor;
					count++;
				}
			}
			string fn3 = "svm-process/eigenfood3.png";
			eigenPic3.save(fn3.c_str());
			
			count = 0;
			high = -10000, low = 10000, shiftAmount = 0, multFactor = 1;
			CImg<double> eigenPic4(smallestDimension, smallestDimension);
			for(int i = 0; i<smallestDimension*smallestDimension; i++) {
				if(eigenMatrix(i,3) < low) low = eigenMatrix(i,3);
				else if (eigenMatrix(i,3) > high) high = eigenMatrix(i,3);
			}
			shiftAmount = low * -1;
			high = high + shiftAmount;
			if(high != 0) multFactor = 255/high;
			
			for(int y = 0; y<smallestDimension; y++) {
				for(int x = 0; x<smallestDimension; x++) {
					if(multFactor == 1) eigenPic4(x,y) = eigenMatrix(count,3);
					else eigenPic4(x,y) = (eigenMatrix(count,3) + shiftAmount)*multFactor;
					count++;
				}
			}
			string fn4 = "svm-process/eigenfood4.png";
			eigenPic4.save(fn4.c_str());
			
			count = 0;
			high = -10000, low = 10000, shiftAmount = 0, multFactor = 1;
			CImg<double> eigenPic5(smallestDimension, smallestDimension);
			for(int i = 0; i<smallestDimension*smallestDimension; i++) {
				if(eigenMatrix(i,4) < low) low = eigenMatrix(i,4);
				else if (eigenMatrix(i,4) > high) high = eigenMatrix(i,4);
			}
			shiftAmount = low * -1;
			high = high + shiftAmount;
			if(high != 0) multFactor = 255/high;
			
			for(int y = 0; y<smallestDimension; y++) {
				for(int x = 0; x<smallestDimension; x++) {
					if(multFactor == 1) eigenPic5(x,y) = eigenMatrix(count,4);
					else eigenPic5(x,y) = (eigenMatrix(count,4) + shiftAmount)*multFactor;
					count++;
				}
			}
			string fn5 = "svm-process/eigenfood5.png";
			eigenPic5.save(fn5.c_str());*/
			
			
		}




		virtual string classify(const string &filename){

			ifstream open_predict_dat;
						open_predict_dat.open("svm-process/part2-eigen-svmPredictionEigen.txt");
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



