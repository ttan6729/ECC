#ifndef ECC_CLUSTERING_H_
#define ECC_CLUSTERING_H_

#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std;


class Cluster
{
public:
	int clusterId;
	int centroid_id;
	vector<int> points_id;

	Cluster() {}
};

class KMedoids
{

private:
    vector<Cluster> clusters;

public:
	int K, iters, dimensions, n_points, final_id;

    vector<vector<double>> data;  //similarity matrix of n samples 
    vector<int> cluster_ids;   //cluster id of each point
    vector<string> list;
    vector<int> centroid_ids;
    KMedoids(int K, int iterations)
    {
        this->K = K;
        this->iters = iterations;
    }

    KMedoids(vector<vector<double>> _data,vector<int> centroid_ids, vector<string> _list, int iterations=50)
    {
    	K = centroid_ids.size();
    	iters = iterations;
    	data = _data;
    	list = _list;
    	n_points = data.size();
    	printf("number of points: %d\n", n_points);
    	for(int i = 0; i < K; i++)
    	{
    		Cluster C;
    		C.clusterId = i;
    		C.centroid_id = centroid_ids[i];
    		clusters.push_back(C);
    	}
    	cluster_ids.resize(data.size(),-1); 
    }

	int getNearestClusterId(int pointId);
	void run();
	void calculate_id();
	void sequential_result(string result="");
    void cluster_result(string result="");
    void pair_result(string result="");

};

class SubtracitveCluster
{
public:
	double alpha;
	double beta;
	SubtracitveCluster(double _alpha=1.0, double _beta=1.5): alpha(_alpha),beta(_beta)
	{}

	vector<int> fit(const vector<vector<double>> &data);
};


#endif