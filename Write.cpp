#include <vector>
#include <fcntl.h>
#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

struct results_file_head {
  char type[8] = "RESULT";
  unsigned long long int trainingID;
  unsigned long long int queryID;
  unsigned long long int resultsID;
  unsigned long long int numOfQueries;
  unsigned long long int numOfDimensions;
  unsigned long long int numOfNeighbors;
};

void writeToFile(
  const vector<vector<pair<double, Node*>>> & knns,
  unsigned long long  n,
  unsigned long long  queries,
  unsigned long long  dimension,
  unsigned long long trainingID,
  unsigned long long queryID) {

  string filename = "results_file.dat";
  int f = open(filename.c_str(), O_RDWR|O_CREAT);
  if (f == -1) {
    perror("Error opening file for writing");
    exit(-1);
  }

  // ssize_t write(int filedes, const void *buf, size_t nbytes);
  string type = "RESULT\n";
  unsigned long long resultsID = 1;
  write(f, &type, sizeof(unsigned long long));
  write(f, &trainingID, sizeof(unsigned long long));
  write(f, &queryID, sizeof(unsigned long long));
  write(f, &resultsID, sizeof(unsigned long long));
  write(f, &queries, sizeof(unsigned long long));
  write(f, &dimension, sizeof(unsigned long long));
  write(f, &n, sizeof(unsigned long long));

  for (int i = 0; i < knns.size(); ++i) {
    for (int j = 0; j < knns[i].size(); ++j){
      write(f, &knns[i][j].first, sizeof(float));
    }
  }
}
