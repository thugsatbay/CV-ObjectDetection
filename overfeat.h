class Overfeat : public Classifier {
				

		public:

				Overfeat(const vector<string> &_class_list) : Classifier(_class_list) {}

				virtual void train(const Dataset &filenames){
						
						int result = system("./folder231.sh ./train ./train231 1 0");
						string directory("train");
						string output_directory("train231");
						DIR *dir = opendir(directory.c_str());
						DIR *sub_dir;
					  	if(!dir)
					    	throw std::string("Can't find directory " + directory);
					  
					  	cout << "$$$ Image Resize and overfeat feature dump $$$" << endl;
					  	struct dirent *dirent, *sub_dirent;
					  	while ((dirent = readdir(dir))) 
					    	if(dirent->d_name[0] != '.'){
								sub_dir = opendir((directory + "/" + dirent->d_name).c_str());
								while ((sub_dirent = readdir(sub_dir)))
									if(sub_dirent->d_name[0] != '.'){
										string file_name(directory + "/" + dirent->d_name + "/" + sub_dirent->d_name);
										string out_file_name(output_directory + "/" + dirent->d_name + "/" + sub_dirent->d_name);
										cout << "Processing " << file_name.c_str() << " ..." << endl;
										CImg<double> asd(file_name.c_str());
										asd.resize(231, 231);
										asd.get_normalize(0, 255).save(out_file_name.c_str());
										//int result = system((string("./overfeat/bin/linux_64/overfeat -L 12 ") + out_file_name + " > dump/ " + dirent->d_name + "/" + sub_dirent->d_name + ".txt").c_str());
										result = system((string("./overfeat/bin/linux_64/overfeat -L 21 ") + out_file_name + " > ./dump/" + dirent->d_name + "/" + sub_dirent->d_name + ".txt").c_str());
									}
							}
					
					ofstream create_train_dat; 
					string line;
					create_train_dat.open("svm-process/part3-train.dat");
					directory = "dump";
					dir = opendir(directory.c_str());
					vector<string> segList;
					string segment;
					int class_target = 0;
					while ((dirent = readdir(dir))){
						if(dirent->d_name[0] != '.'){
							sub_dir = opendir((directory + "/" + dirent->d_name).c_str());
							class_target += 1;
							while ((sub_dirent = readdir(sub_dir))){
								if(sub_dirent->d_name[0] != '.'){
									string file_name(directory + "/" + dirent->d_name + "/" + sub_dirent->d_name);
									ifstream open_class_data(file_name.c_str());
									if ( open_class_data.is_open()){
										while ( getline (open_class_data,line) ){
											if (line.size() > 15){
												create_train_dat << (toString(class_target) + " ").c_str();
												segList.clear();
												stringstream sss;
												sss.str(line);
												while(getline(sss, segment, ' '))
												{
												   segList.push_back(segment);
												}
												for (unsigned int loop = 0; loop < segList.size(); ++loop){
													create_train_dat << (loop + 1) << ":" << segList[loop] << " "; 
												}
												create_train_dat << "#" << file_name.c_str() << endl;
											}
										}
										open_class_data.close();
									}
									cout << "Processed to part3-train.dat; File : " << file_name.c_str() << endl;
								}
							}
						}
					}
					create_train_dat.close();
					
					result = system("./svm_multiclass_learn -c 5000 svm-process/part3-train.dat svm-process/part3-model.dat");
				}


				virtual string classify(const string &filename){
						ifstream open_predict_dat;
						open_predict_dat.open("svm-process/part3-prediction.dat");
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
				


				virtual void load_model(const Dataset &filenames){
						
						int result = system("./folder231.sh ./test ./test231 1 1");
						int index_file = 1;
						string directory("test");
						string output_directory("test231");
						DIR *dir = opendir(directory.c_str());
						DIR *sub_dir;
					  	if(!dir)
					    	throw std::string("Can't find directory " + directory);
					  
					  	cout << "$$$ Image Resize and overfeat feature dump $$$" << endl;
					  	struct dirent *dirent, *sub_dirent;
					  	while ((dirent = readdir(dir))) 
					    	if(dirent->d_name[0] != '.'){
								sub_dir = opendir((directory + "/" + dirent->d_name).c_str());
								while ((sub_dirent = readdir(sub_dir)))
									if(sub_dirent->d_name[0] != '.'){
										string file_name(directory + "/" + dirent->d_name + "/" + sub_dirent->d_name);
										string out_file_name(output_directory + "/" + dirent->d_name + "/" + sub_dirent->d_name);
										cout << "Processing " << file_name.c_str() << " ..." << endl;
										CImg<double> asd(file_name.c_str());
										asd.resize(231, 231);
										asd.get_normalize(0, 255).save(out_file_name.c_str());
										//int result = system((string("./overfeat/bin/linux_64/overfeat -L 12 ") + out_file_name + " > dump/ " + dirent->d_name + "/" + sub_dirent->d_name + ".txt").c_str());
										result = system((string("./overfeat/bin/linux_64/overfeat -L 21 ") + out_file_name + " > ./ttdump/" + dirent->d_name + "/" + sub_dirent->d_name).c_str());
									}
							}
					
					ofstream create_train_dat; 
					string line;
					create_train_dat.open("svm-process/part3-test.dat");
					directory = "ttdump";
					dir = opendir(directory.c_str());
					vector<string> segList;
					string segment;
					int class_target = 0;
					while ((dirent = readdir(dir))){
						if(dirent->d_name[0] != '.'){
							sub_dir = opendir((directory + "/" + dirent->d_name).c_str());
							class_target += 1;
							image_class_number[toString(class_target)] = dirent->d_name;
							cout << "Class : " << dirent->d_name << " has class number : " << class_target << endl;
							while ((sub_dirent = readdir(sub_dir))){
								if(sub_dirent->d_name[0] != '.'){
									string file_name(directory + "/" + dirent->d_name + "/" + sub_dirent->d_name);
									string t_file_name(string("test/") + dirent->d_name + "/" + sub_dirent->d_name);
									cout << t_file_name << ", Index : " << index_file;
									test_file_number[t_file_name] = index_file++;
									ifstream open_class_data(file_name.c_str());
									if ( open_class_data.is_open()){
										while ( getline (open_class_data,line) ){
											if (line.size() > 15){
												create_train_dat << (toString(class_target) + " ").c_str();
												segList.clear();
												stringstream sss;
												sss.str(line);
												while(getline(sss, segment, ' '))
												{
												   segList.push_back(segment);
												}
												for (unsigned int loop = 0; loop < segList.size(); ++loop){
													create_train_dat << (loop + 1) << ":" << segList[loop] << " "; 
												}
												create_train_dat << "#" << file_name.c_str() << endl;
											}
										}
										open_class_data.close();
									}
									cout << "Processed to part3-test.dat; File : " << file_name.c_str() << " For Class : " << dirent->d_name <<endl;
								}
							}
						}
					}
					create_train_dat.close();
					
					result = system("./svm_multiclass_classify svm-process/part3-test.dat svm-process/part3-model.dat svm-process/part3-prediction.dat");
				}
};
					

