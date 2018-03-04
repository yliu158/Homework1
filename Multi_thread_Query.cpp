#include "Multi_thread_Build.cpp"

void queries(
  vector<vector<double> >* data,
  Node* current,
  vector<vector<double> >* query_points,
  vector<vector<pair<double, Node*> > >* knns,
  int start, int size, int neighbors_num) {

  // initialize knn container
  vector< pair<double, Node*> > knn_single_query(neighbors_num, pair<double, Node*>(numeric_limits<double>::max() , NULL));

  // for each query in the range
  int end = start+size;
  if (end > query_points->size()) end = query_points->size();

  for (int i = start; i < end; ++i) {
    // get the initialized prototype
    vector< pair<double, Node*> > knn(knn_single_query);
    single_query(*data, current, query_points->at(i), knn);
    knns->at(i) = knn;

  }
  knn_single_query.clear();
  vector< pair<double, Node*> >().swap(knn_single_query);
  return;
}

void multi_thread_query(vector<vector<double> >& data, Node* current, vector<vector<double> > * query_points, vector<vector<pair<double, Node*> > >& knns, int core_num, int neighbors_num) {
  int size_of_partition = (query_points->size()+core_num-1)/core_num;
  knns = vector<vector<pair<double, Node*>>>(query_points->size());
  vector<thread> threads(core_num);
  int start = 0;
  for (int i = 0; i < core_num; ++i) {
    threads[i] = thread(queries, &data, current, query_points, &knns, start, size_of_partition, neighbors_num);
    start += size_of_partition;
  }
  for (int i = 0; i < core_num; ++i) {
    threads[i].join();
  }
}
