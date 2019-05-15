#include "clustering.h"


int KMedoids::getNearestClusterId(int pointId)
{
	double min = data[pointId][ clusters[0].centroid_id ];
	int result = 0;
	for(int i = 1; i < K; i++)
	{
		if (min > data[pointId][ clusters[i].centroid_id ])
		{
			min = data[pointId][ clusters[i].centroid_id ];
			result = i;
		}
	}
	return result;
}


void KMedoids::run()
{
    printf("max iters %d\n",iters);
    vector<int> previous_ids = cluster_ids; 
    calculate_id();
    int iter = 0;
    while(iter++ < iters && previous_ids != cluster_ids)
    {
        printf("iter %d\n",iter);
        previous_ids = cluster_ids;
        calculate_id(); 
    }
//    printf("result:\n");
//    for(int i = 0; i < cluster_ids.size(); i++)
//      printf("point %d, cluster %d\n",i+1,cluster_ids[i]);

    double min_dist = 0;
    double min_id = 0;
    for(int i = 0; i < clusters.size(); i++)
    {
        printf("number of samples in cluster %d: %d\n",i,clusters[i].points_id.size());
        centroid_ids.push_back( clusters[i].centroid_id );
        double sum = 0.0;
        for(int j = 0; j < clusters.size(); j++)
            //sum += pow(data[ clusters[i].centroid_id ][clusters[j].centroid_id],2);
            sum += exp(data[ clusters[i].centroid_id ][clusters[j].centroid_id]);
        
        if( min_dist > sum || min_dist == 0)
        {
            min_dist = sum;
            min_id = i;
        }
    }
    final_id = clusters[min_id].centroid_id;      


    
}


void KMedoids::calculate_id()
{
	int n = data.size();
    for(int i = 0; i < clusters.size(); i++)
        clusters[i].points_id.clear();

    for(int i = 0; i < n_points; i++)
    {
//        int currentClusterId = all_points[i].getCluster();
        cluster_ids[i] = getNearestClusterId(i);
        clusters[ cluster_ids[i] ].points_id.push_back(i);
    }  
	//recalculate the center of each cluster
    for(int i = 0; i < clusters.size(); i++)
    {
    	double min_dist = 0.0;
    	double min_id = 0;
    	for(int j = 0; j < clusters[i].points_id.size(); j++)
    	{
    		double sum = 0.0;
    		for(int k = 0; k < clusters[i].points_id.size(); k++)
                sum += pow(data[clusters[i].points_id[j]][clusters[i].points_id[k]],2);
    			//sum += exp(data[clusters[i].points_id[j]][clusters[i].points_id[k]]);
    		
    		if( min_dist > sum || min_dist == 0.0)
    		{
    			min_dist = sum;
    			min_id = clusters[i].points_id[j];
    		}
    	}
    	clusters[i].centroid_id = min_id;
    }
 
    return;
}

int max_index(std::vector<double> v)
{
    double max = v[0];
    int index = 0;
    for( int i = 0; i < v.size(); i++)
    {
        if( v[i] > max)
        {
            max = v[i];
            index = i;
        }
    }

    return index;
}


int max_index(std::vector<double> v, std::vector<int> existing_index)
{
    double max;
    int index = -1;
    for( int i = 0; i < v.size(); i++)
    {
        if ( find(existing_index.begin(),existing_index.end(),i) != existing_index.end() )
            continue;
        if (index == -1)
        {
            index = i;
            max = v[i];
        }
        
        else if( v[i] > max)
        {
            max = v[i];
            index = i;
        }
    }


    return index;
}
//input: similarity matrix with diemsnion n*n
vector<int> SubtracitveCluster::fit(const vector<vector<double>> &data)
{
	int i, j, n = data.size();
	vector<int> centroids;
	vector<double> potential_values(n,0.0);

    for(i = 0; i < n; i++)
    {
    	double mountain_coefficient = 0.0;
    	for( j = 0; j < n; j++)
    		mountain_coefficient += exp(-alpha*data[i][j]);

    	potential_values[i] = mountain_coefficient;	 
    }

    int current_centroid = max_index(potential_values);
    int max_clusters =   0.25*sqrt(n) > 4 ? 0.15*sqrt(n) : 4;
    double threshold = 0.7 * potential_values[current_centroid];

    while(potential_values[current_centroid] >= threshold && centroids.size() <= max_clusters)
    {
    	for(i = 0; i < n; i++)
    	{
    		if (find(centroids.begin(),centroids.end(),i) == centroids.end() )  //ith point is not in centroid set
    			potential_values[i] -= exp(-alpha*data[i][current_centroid]);
    	}	
    	current_centroid = max_index(potential_values,centroids);
    	centroids.push_back(current_centroid);
    }
    return centroids;
}

void KMedoids::cluster_result(string result)
{
	if( list.size() != cluster_ids.size() )
	{ 
		printf("error, size of input matrix and list doesn't match\n");
		return;
	}

	if(result == "")
		result = "ECCResult";

	ofstream fp;
	fp.open((result+"_cluster.txt").c_str());

	fp  << final_id << ' ';
	for(int i = 0; i < clusters.size(); i++)
		fp << clusters[i].centroid_id <<  ' ';
	fp << "\n";


	for(int i = 0; i < list.size(); i++)
		fp << list[i] << ' ' << cluster_ids[i] << "\n";


	fp.close();
}

void KMedoids::sequential_result(string result)
{
    if( list.size() != cluster_ids.size() )
    { 
        printf("error, size of input matrix and list doesn't match\n");
        return;
    }

    if(result == "")
        result = "ECCSeqResult";

    ofstream fp;
    fp.open((result+"_seq.txt").c_str());

    fp  << list[final_id] << "\n";
    for(int i = 0; i < clusters.size(); i++)
    {
        if ( clusters[i].centroid_id != final_id )
            fp << list[clusters[i].centroid_id] << "\n";
    }

    for(int i = 0; i < clusters[ cluster_ids[final_id]].points_id.size(); i++ )
    {
        if( clusters[ cluster_ids[final_id]].points_id[i] != final_id )
             fp << list[ clusters[ cluster_ids[final_id]].points_id[i] ] << "\n";
    }

    for(int i = 0; i < clusters.size(); i++)
    {
        if ( i != cluster_ids[final_id])
        {
            for(int j = 0; j < clusters[i].points_id.size(); j++)
            {
                if( clusters[i].points_id[j] != clusters[i].centroid_id )
                    fp << list[clusters[i].points_id[j]] << "\n";
            }
        }
    }
    fp.close();
}

void KMedoids::pair_result(string result)
{

    ofstream fp;
    fp.open((result+"_pair.txt").c_str());
    //write information of main cluster first
    Cluster c = clusters[ cluster_ids[final_id] ];
    for(int i = 0; i < c.points_id.size(); i++)
    {
        if ( c.points_id[i] != c.centroid_id )
            fp << list[ c.centroid_id ] << " " << list[c.points_id[i]] << "\n";
    }
    //write information of other centroids
    for(int i = 0; i < clusters.size(); i++)
    {
        if ( clusters[i].centroid_id != final_id)
            fp << list[ c.centroid_id ] << " " << list[ clusters[i].centroid_id ] << "\n" ; 
    }
    //write information of remaining clusters
    for(int i = 0; i < clusters.size(); i++)
    {
        c = clusters[i];
        if( c.centroid_id != final_id)
        {
            for(int j = 0; j < c.points_id.size(); j++)
            {
                if( c.points_id[j] != c.centroid_id )
                    fp << list[c.centroid_id] << " " << list[c.points_id[j]] << "\n";
            }
        }
    }

    fp.close();
    return;
}
