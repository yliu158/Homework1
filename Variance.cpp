#include <iostream>
#include <vector>
#include <numeric>
#include <math.h>
using namespace std;

double variance (const vector<double>& v) {
  double sum = accumulate(v.begin(), v.end(), 0.0);
  double mean = sum / v.size();
  vector<double> diff(v.size());
  transform(v.begin(), v.end(), diff.begin(), bind2nd(minus<double>(), mean));
  double sq_sum = inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
  double stdev = sqrt(sq_sum / v.size());
  return stdev;
}

bool compareFuncVar(const pair<double, int>& left, const pair<double, int>& right) {
  return left.first > right.first;
}

void preProcess(vector<vector<double> >& data) {
  vector<pair<double, int> > vars;
  int row_num = data.size();
  int col_num = data[0].size();

  vector<double> col;
  for (int i = 0; i < col_num; ++i) {
    col.clear();
    vector<double>().swap(col);
    for (int j = 0; j < row_num; ++j) {
      col.push_back(data[j][i]);
    }
    double var = variance(col);
    vars.push_back(make_pair(var, i));
  }
  sort(vars.begin(), vars.end(), compareFuncVar);
  vector<vector<double> > replace_data(row_num, vector<double>(col_num, 0));
  for (int i = 0; i < row_num; ++i) {
    for (int j = 0; j < col_num; ++j) {
      int col_index = vars[j].second;
      replace_data[i][j] = data[i][col_index];
    }
  }
  data.clear();
  vector<vector<double> >().swap(data);
  data = replace_data;
  cout <<"PreProcessed\n";
  return;
}
