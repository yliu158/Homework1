#include "Multi_thread_Query.cpp"
#include <limits.h>

#ifndef GET_TSC_HPP
#define GET_TSC_HPP

inline unsigned long long
get_tsc() {
    unsigned int a, d;
    // asm volatile("lfence; rdtsc; lfence" : "=a" (a), "=d" (d));
    asm volatile("rdtsc" : "=a" (a), "=d" (d));
    return (unsigned long) a | (((unsigned long) d) << 32);;
}

#endif

void showResults(int core_num, string filename, string query_filename) {
  vector<vector<double> > *data = readTrainingFile(filename);
  Node* root = buildTree(*data);
  int neighbors = 0;
  vector<vector<double> > * queries = readQueryFile(query_filename, neighbors);
  vector<vector<pair<double, Node*>>> knns_kd;
  multi_thread_query(*data, root, queries, knns_kd, core_num, neighbors);
  for (int i = 0; i < knns_kd.size(); ++i) {
    cout << i + 1<< "th query.\n";
    for (int j = 0; j < knns_kd[i].size(); ++j) {
      cout << setw(10) <<setfill(' ') << left << knns_kd[i][j].first << "     ";
    }
    cout << endl;
  }
}

int main(int argc, char const *argv[]) {
    if (argc < 4) {
        cout << "Fail to get arguments." << endl;
        exit(0);
    }
  int core_num = atoi(argv[1]);
  string training_file(argv[2]);
  string query_file(argv[3]);
  showResults(core_num, training_file, query_file);
  return 0;
}
