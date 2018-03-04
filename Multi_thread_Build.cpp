#include "Read.cpp"
#include "Variance.cpp"
#include "KD_Tree.h"
using namespace std;

class Node {
public:
  Node* left;
  Node* right;
  int index;
  int dimension;
  Node (int index ,int dimension) {
    this->index = index;
    this->dimension = dimension;
    left = NULL;
    right = NULL;
  }
};

struct Parameters{
  vector<vector<double> >* data;
  int begin;
  int end;
  int dimension;
};

void printData(const vector<vector<double> >& data) {
  cout << endl;
  for (vector<double> v: data) {
    for (double d: v) {
      cout << left <<setfill(' ') << setw(10) << left << d;
    }
    cout << endl;
  }
  cout << endl;
}

bool compareFunc(const vector<double>& first, const vector<double>& second) {
  assert(first.back() == second.back());
  int dim_index = first.back();
  return first[dim_index%(first.size()-1)] < second[dim_index%(second.size()-1)];
}

void localSort(vector<vector<double> >& v, int begin, int end, int dimension) {
  for (int i = begin; i <= end; ++i) {
    v[i].back() = dimension;
  }
  sort(v.begin()+begin, v.begin()+end+1, compareFunc);
}

void prepareTreeHelper(vector<vector<double> >& data, int begin, int end, int dimension) {
  if (begin >= end) return;
  localSort(data, begin, end, dimension);
  int mid = (begin+end)/2;
  prepareTreeHelper(data, begin, mid-1, dimension+1);
  prepareTreeHelper(data, mid+1, end, dimension+1);
  return;
}

void multi_thread_prepareTreeHelper(vector<vector<double> >* data, int begin, int end, int dimension, const int& core_num) {
  if (begin >= end) return;
  // cout << "Multi_thread.\n" ;
  localSort(*data, begin, end, dimension);
  int mid = (begin+end)/2;
  if (pow(2, dimension+1) > core_num) {
    // single thread
    prepareTreeHelper(*data, begin, mid-1, dimension+1);
    prepareTreeHelper(*data, mid+1, end, dimension+1);
  } else {
    thread t1(multi_thread_prepareTreeHelper, data, begin, mid-1, dimension+1, core_num);
    thread t2(multi_thread_prepareTreeHelper, data, mid+1, end, dimension+1, core_num);
    t1.join();
    t2.join();
  }
  return;
}

void prepareTree(vector<vector<double> >& data) {
  preProcess(data);
  for (int i = 0; i < data.size(); ++i) {
    data[i].push_back(0);
  }
  multi_thread_prepareTreeHelper(&data, 0, data.size()-1, 0, 4);
  for (int i = 0; i < data.size(); ++i) {
    data[i].pop_back();
  }
  return;
}

Node* buildTreeHelper(const vector<vector<double> >& data, int begin, int end, int dimension) {
  if (begin > end) return NULL;
  if (begin == end) {
    return new Node(begin, dimension);
  }
  int mid = (begin+end)/2;
  Node* cur = new Node(mid, dimension);
  cur->left = buildTreeHelper(data, begin, mid-1, dimension+1);
  cur->right = buildTreeHelper(data, mid+1, end, dimension+1);
  return cur;
}

void multi_thread_buildTreeHelper(vector<vector<double> >* data, Node* parent, bool left, int begin, int end, int dimension, const int& core_num) {
  // cout << "multi_thread_buildTreeHelper.\n";
  if (begin > end) return;
  if (begin == end) {
    if (left) {
      parent->left = new Node(begin, dimension);
    } else {
      parent->right = new Node(begin, dimension);
    }
    return;
  }

  int mid = (begin+end)/2;
  Node* cur = new Node(mid, dimension);
  if (left) {
    parent->left = cur;
  } else {
    parent->right = cur;
  }
  if (pow(2, dimension+1) > core_num ) {
    // single thread
    cur->left = buildTreeHelper(*data, begin, mid-1, dimension+1);
    cur->right = buildTreeHelper(*data, mid+1, end, dimension+1);
  } else {
    thread left(multi_thread_buildTreeHelper, data, cur, true, begin, mid-1, dimension+1, core_num);
    thread right(multi_thread_buildTreeHelper, data, cur, false, mid+1, end, dimension+1, core_num);
    left.join();
    right.join();
  }
}

Node* buildTree(vector<vector<double> >& data) {
  prepareTree(data);
  // Node* root = buildTreeHelper(data, 0, data.size()-1,0);
  Node * root;
  int begin = 0;
  int end = data.size()-1;
  int dimension = 0;
  int core_num = 4;

  if (begin > end) return NULL;
  if (begin == end) {
    root = new Node(begin, dimension);
    return root;
  }
  // begin < end;
  int mid = (begin+end)/2;
  root = new Node(mid, dimension);
  if (core_num >= 2) {
    multi_thread_buildTreeHelper(&data, root, true, begin, mid-1, dimension+1, core_num);
    multi_thread_buildTreeHelper(&data, root, false, mid+1, end, dimension+1, core_num);
  } else {
    root->left = buildTreeHelper(data, begin, mid-1, dimension+1);
    root->right = buildTreeHelper(data, mid+1, end, dimension+1);
  }
  return root;
}

void printTree(const vector<vector<double> >& data, Node* root) {
  if (root == NULL) return;
  printTree(data, root->left);
  for (int i = 0; i < root->dimension; ++i) {
    cout << "      ";
  }
  cout << data[root->index][root->dimension%data[root->index].size()] << endl;
  printTree(data, root->right);
  return;
}


vector<vector<double> > randomData(int size_of_data, int dimension) {
  vector<vector<double> > v(size_of_data, vector<double>(dimension, 1));
  for (int i = 0; i < v.size(); ++i) {
    for (int j = 0; j < v[i].size(); ++j) {
      int sign = (rand()%2);
      if (sign) {
        v[i][j] = (double)((rand()%1000000)/(double)1000);
      } else {
        v[i][j] = -(double)((rand()%1000000)/(double)1000);
      }
    }
  }
  return v;
}

double euclideanMetric(const vector<double>& first, const vector<double>& second) {
  assert(first.size() == second.size());
  double distance = 0;
  for (int i = 0; i < first.size(); ++i ) {
    distance += first[i]*first[i] + second[i]*second[i];
  }
  return sqrt(distance);
}

void single_query(const vector<vector<double> >& data, Node* current, const vector<double>& p, vector<pair<double, Node*> >& knn) {
  if (current == NULL) return;
  double r = knn.back().first;
  double d = euclideanMetric(p, data[current->index]);
  if (d < r) {
    knn.back().first = d;
    knn.back().second = current;
    sort(knn.begin(), knn.end());
  }
  r = knn.back().first;
  int dim_index = current->dimension%(data[current->index].size());
  double cuttingLine = data[current->index][dim_index];
  bool first_check_left = false;
  if (p[dim_index] <= cuttingLine) {
    single_query(data, current->left, p, knn);
    first_check_left = true;
  } else {
    single_query(data, current->right, p, knn);
  }

  double d_cross_dim = abs(cuttingLine-p[dim_index]);
  if (first_check_left) {
    if (d_cross_dim < knn.back().first) {
      single_query(data, current->right, p, knn);
    }
  } else {
    if (d_cross_dim < knn.back().first) {
      single_query(data, current->left, p, knn);
    }
  }
  return;
}

void destructTree(Node* current) {
  if (current == NULL) return;
  destructTree(current->left);
  destructTree(current->right);
  delete current;
}
