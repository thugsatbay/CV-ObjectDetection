class SVM : public Classifier {

		protected:
				static const int size = 75;
				static const int color = 1;
				

		public:

				SVM(const vector<string> &_class_list) : Classifier(_class_list) {}

				virtual void train(const Dataset &filenames){
						ofstream create_train_dat;

						create_train_dat.open("svm-process/part1-train.dat");
						int class_target = 0;
						for(Dataset::const_iterator c_iter = filenames.begin(); c_iter != filenames.end(); ++c_iter){
								class_target += 1;
								for(unsigned int loop = 0; loop < c_iter->second.size(); ++loop){
									//cout << c_iter->second[loop] << endl;
									CImg<double> image(c_iter->second[loop].c_str());
									if (color == 0)
										image = image.get_RGBtoHSI().get_channel(2).resize(size, size, 1, 1).unroll('x').get_normalize(0,255);
									else
										image = image.resize(size, size).unroll('x').get_normalize(0, 255);
									//cout << image.width() << " " << image.height() << endl;
									create_train_dat << class_target << " ";
									for(int row = 0; row < image.width(); ++row)
											create_train_dat << (row + 1) << ":" << image(row, 0, 0, 0) << " ";
									create_train_dat << "# " << c_iter->second[loop] << endl;
							}
							cout << c_iter->first.c_str() << " Image Folder Processed" << endl;
						}
						int result = system("./svm_multiclass_learn -c 5000 svm-process/part1-train.dat svm-process/part1-model.dat");
				}


				virtual string classify(const string &filename){
						ifstream open_predict_dat;
						open_predict_dat.open("svm-process/part1-prediction.dat");
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
						ofstream create_test_dat;
						create_test_dat.open("svm-process/part1-test.dat");
						int class_target = 0;
						int index_file = 1;
						for(Dataset::const_iterator c_iter = filenames.begin(); c_iter != filenames.end(); ++c_iter){
								class_target += 1;
								for(unsigned int loop = 0; loop < c_iter->second.size(); ++loop){
									//cout << c_iter->second[loop] << endl;
									image_class_number[toString(class_target)] = c_iter->first;
									test_file_number[c_iter->second[loop]] = index_file++ ;
									CImg<double> image(c_iter->second[loop].c_str());
									if (color == 0)
										image = image.get_RGBtoHSI().get_channel(2).resize(size, size, 1, 1).unroll('x').get_normalize(0,255);
									else
										image = image.resize(size, size).unroll('x').get_normalize(0, 255);
									//cout << image.width() << " " << image.height() << endl;
									create_test_dat << class_target << " ";
									for(int row = 0; row < image.width(); ++row)
											create_test_dat << (row + 1) << ":" << image(row, 0, 0, 0) << " ";
									create_test_dat << "# " << c_iter->second[loop] << endl;
							}
							cout << c_iter->first.c_str() << " Image Folder Processed" << endl;
						}
						int result = system("./svm_multiclass_classify svm-process/part1-test.dat svm-process/part1-model.dat svm-process/part1-prediction.dat");
				}
};
					

