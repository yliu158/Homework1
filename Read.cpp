#include <vector>
#include <fcntl.h>
#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string>
using namespace std;

struct training_file_head {
  char type[8];
  unsigned long long int trainingID;
  unsigned long long int numOfPOints;
  unsigned long long int numOfDimensions;
};

struct query_file_head {
  char type[8];
  unsigned long long int queryID;
  unsigned long long int numOfQueries;
  unsigned long long int numOfDimensions;
  unsigned long long int numOfNeighbors;
};

vector<vector<double> >* readTrainingFile(const string& filename) {
  int f = open(filename.c_str(), O_RDONLY);
  if (f == -1) {
    cout << "Fail to read the training_file and Exited." << endl;
    exit(-1);
  }
  struct stat st;
  int fileSize = fstat(f, &st);
  if (fileSize == -1) {
    cout << "Fail to get file size." << endl;
    close(f);
    exit(-1);
  }
  struct training_file_head head;
  int len = st.st_size;
  read(f, &head, sizeof(struct training_file_head));
  int dimension = head.numOfDimensions;
  int points = head.numOfPOints;
  vector<vector<double> >* data = new vector<vector<double> >(points/dimension, vector<double>(dimension, 0));
  int row = 0;
  int col = 0;
  float num = 0;
  for (int i = 0; i < points; ++i) {
    row = i/dimension;
    col = i%dimension;
    read(f, &num, sizeof(float));
    (*data)[row][col] = num;
  }
  close(f);
  return data;
}

vector<vector<double> >* readQueryFile(const string& filename, int& n) {
  int f = open(filename.c_str(), O_RDONLY);
  if (f == -1) {
    cout << "Fail to read query file and Exited.\n";
    exit(-1);
  }
  struct stat st;
  int fileSize = fstat(f, &st);
  if (fileSize == -1) {
    cout << "Fail to get file size." << endl;
    close(f);
    exit(-1);
  }
  int len = st.st_size;
  struct query_file_head head;
  read(f, &head, sizeof(struct query_file_head));
  int querys_num = head.numOfQueries;
  int dimension = head.numOfDimensions;
  n = head.numOfNeighbors;
  vector<vector<double> >* query = new vector<vector<double> >();
  vector<double> row;
  float num = 0;
  for (int i = 0; i < querys_num; ++i) {
    row.clear();
    vector<double>().swap(row);
    for (int j = 0; j < dimension; ++j) {
      read(f, &num, sizeof(float));
      row.push_back(num);
    }
    query->push_back(row);
  }
  return query;
}
