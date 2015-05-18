#include "lda.h"
#include <mmintrin.h>

int simpleLDA::specific_init()
{
	return 0;
}

int simpleLDA::sampling(int i)
{
	std::mt19937 urng(i);
	std::uniform_real_distribution<double> d_unif01(0.0, 1.0);
	
	double * p = new double[K]; // temp variable for sampling
	int *nd_m = new int[K];
	int *rev_mapper = new int[K];
	for (int k = 0; k < K; ++k)
	{
		nd_m[k] = 0;
		rev_mapper[k] = -1;
	}
	std::chrono::high_resolution_clock::time_point ts, tn;
	
	for (int iter = 1; iter <= niters; ++iter)
	{
		ts = std::chrono::high_resolution_clock::now();
		// for each document of worker i
		for (int m = i; m < M; m+=nst)
		{
			int kc = 0;
			for (const auto& k : nds1[m])
			//for (auto k = nds1[m].begin(); k != nds1[m].end(); ++k)
			{
				nd_m[k.first] = k.second;
				rev_mapper[k.first] = kc++;
			}
			for (int n = 0; n < ptrndata->docs[m]->length; ++n)
			{
				int w = ptrndata->docs[m]->words[n];
				
				// remove z_ij from the count variables
				int topic = z[m][n]; int old_topic = topic;
				nd_m[topic] -= 1;
				nds1[m][rev_mapper[topic]].second -= 1;

				// do multinomial sampling via cumulative method
				double temp = 0;
				for (int k = 0; k < K; k++)
				{
					temp += (nw[w][k] + beta) / (nwsum[k] + Vbeta) *
						(nd_m[k] + alpha);
					p[k] = temp;
				}
				
				// scaled sample because of unnormalized p[]
				double u = d_unif01(urng) * temp;

				// Do a binary search instead!
				topic = std::lower_bound(p, p + K, u) - p;
				
				// add newly estimated z_i to count variables
				if (topic!=old_topic)
				{
					if(nd_m[topic] == 0)
					{
						rev_mapper[topic] = nds1[m].size();
						nds1[m].push_back(std::pair<int, int>(topic, 1));
					}
					else
					{
						nds1[m][rev_mapper[topic]].second += 1;
					}
					nd_m[topic] += 1;
					if (nd_m[old_topic] == 0)
					{
						nds1[m][rev_mapper[old_topic]].first = nds1[m].back().first;
						nds1[m][rev_mapper[old_topic]].second = nds1[m].back().second;
						rev_mapper[nds1[m].back().first] = rev_mapper[old_topic];
						nds1[m].pop_back();
						rev_mapper[old_topic] = -1;
					}
				
					cbuff[nst*(w%ntt)+i].push(delta(w,old_topic, topic));
				}
				else
				{
					nds1[m][rev_mapper[topic]].second += 1;
					nd_m[topic] += 1;
				}
				z[m][n] = topic;
			}
			for (const auto& k : nds1[m])
			{
				nd_m[k.first] = 0;
				rev_mapper[k.first] = -1;
			}
		}
		tn = std::chrono::high_resolution_clock::now();
		std::cout << "In thread " << i << " at iteration " << iter << " ..." 
				  << "Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(tn - ts).count() << std::endl;
	}
	
	delete[] p;
	delete[] nd_m;
	delete[] rev_mapper;
	return 0;
}

int unifLDA::specific_init()
{
	return 0;
}

int unifLDA::sampling(int i)
{
	std::mt19937 urng(i);
	std::uniform_real_distribution<double> d_unif01(0.0, 1.0);
	
	double * p = new double[K]; // temp variable for sampling
	int *nd_m = new int[K];
	int *rev_mapper = new int[K];
	for (int k = 0; k < K; ++k)
	{
		nd_m[k] = 0;
		rev_mapper[k] = -1;
	}
	std::chrono::high_resolution_clock::time_point ts, tn;
	
	for (int iter = 1; iter <= niters; ++iter)
	{
		ts = std::chrono::high_resolution_clock::now();
		// for each document of worker i
		for (int m = i; m < M; m+=nst)
		{
			int kc = 0;
			for (const auto& k : nds1[m])
			//for (auto k = nds1[m].begin(); k != nds1[m].end(); ++k)
			{
				nd_m[k.first] = k.second;
				rev_mapper[k.first] = kc++;
			}
			for (int n = 0; n < ptrndata->docs[m]->length; ++n)
			{
				int w = ptrndata->docs[m]->words[n];
						
				// remove z_ij from the count variables
				int topic = z[m][n]; int new_topic; int old_topic = topic;
				nd_m[topic] -= 1;
				nds1[m][rev_mapper[topic]].second -= 1;

				// MH to draw new topic
				for (int r = 0; r < MH_STEPS; ++r)
				{
					// 1. Draw from the uniform proposal
					new_topic = (int)(d_unif01(urng)*K);
					
					//2. Find acceptance probability
					double temp_old = (nd_m[topic] + alpha) * (nw[w][topic] + beta) / (nwsum[topic] + Vbeta);
					double temp_new = (nd_m[new_topic] + alpha) * (nw[w][new_topic] + beta) / (nwsum[new_topic] + Vbeta);
					double acceptance =  (temp_new) / (temp_old);
					
					//3. Compare against uniform[0,1]
					if (d_unif01(urng) < acceptance)
					{
						topic = new_topic;
					}
				}

				// add newly estimated z_i to count variables
				if (topic!=old_topic)
				{
					if(nd_m[topic] == 0)
					{
						rev_mapper[topic] = nds1[m].size();
						nds1[m].push_back(std::pair<int, int>(topic, 1));
					}
					else
					{
						nds1[m][rev_mapper[topic]].second += 1;
					}
					nd_m[topic] += 1;
					if (nd_m[old_topic] == 0)
					{
						nds1[m][rev_mapper[old_topic]].first = nds1[m].back().first;
						nds1[m][rev_mapper[old_topic]].second = nds1[m].back().second;
						rev_mapper[nds1[m].back().first] = rev_mapper[old_topic];
						nds1[m].pop_back();
						rev_mapper[old_topic] = -1;
					}
				
					cbuff[nst*(w%ntt)+i].push(delta(w,old_topic,topic));
				}
				else
				{
					nds1[m][rev_mapper[topic]].second += 1;
					nd_m[topic] += 1;
				}
				z[m][n] = topic;
			}
			for (const auto& k : nds1[m])
			{
				nd_m[k.first] = 0;
				rev_mapper[k.first] = -1;
			}
		}
		tn = std::chrono::high_resolution_clock::now();
		std::cout << "In thread " << i << " at iteration " << iter << " ..." 
				  << "Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(tn - ts).count() << std::endl;
	}
	
	delete[] p;
	delete[] nd_m;
	delete[] rev_mapper;
	return 0;
}


int sparseLDA::specific_init()
{
	std::cout << "Converting to sparseLDA data structure ..." << std::endl;
	nws.resize(V);
	for (int v = 0; v < V; ++v)
	{
		for (int k = 0; k < K; ++k)
		{
			if (nw[v][k] != 0)
				nws[v].push_back(std::pair<int, int >(nw[v][k],k));
		}
		sort(nws[v].begin(), nws[v].end(), std::greater<std::pair<int, int>>());
	}
		
	return 0;
}

// int sparseLDA::sampling(int i)
// {
	// std::mt19937 urng(i);
	// std::uniform_real_distribution<double> d_unif01(0.0, 1.0);
	
	// double * p = new double[K]; // temp variable for sampling
	// int *nd_m = new int[K];
	// int *rev_mapper = new int[K];
	// for (int k = 0; k < K; ++k)
	// {
		// nd_m[k] = 0;
		// rev_mapper[k] = -1;
	// }
	
	// int *nwsum_local = new int[K];
	// for (int k = 0; k < K; ++k)
	// {
		// nwsum_local[k] = nwsum[k];
	// }
	
	// double ssum, rsum, qsum;
	// double *q1 = new double[K];
	// // compute ssum
	// ssum = 0;
	// for (int k = 0; k < K; ++k)
	// {
		// q1[k] = alpha / (nwsum_local[k] + Vbeta);
		// ssum += 1 / (nwsum_local[k] + Vbeta);
	// }
	// ssum *= alpha * beta;
	// std::chrono::high_resolution_clock::time_point ts, tn;
	
	// for (int iter = 1; iter <= niters; ++iter)
	// {
		// ts = std::chrono::high_resolution_clock::now();
		// // for each document of worker i
		// for (int m = i; m < M; m+=nst)
		// {
			// //synchronization
			// if (m % 100 == i)
			// {
				// for (int k = 0; k < K; ++k)
				// {
					// nwsum_local[k] = nwsum[k];
					// q1[k] = alpha / (nwsum_local[k] + Vbeta);
					// ssum += 1 / (nwsum_local[k] + Vbeta);
				// }
				// ssum *= alpha * beta;
			// }
			
			// int kc = 0;
			// rsum = 0;
			// for (const auto& k : nds1[m])
			// {
				// nd_m[k.first] = k.second;
				// rev_mapper[k.first] = kc++;
				// double temp = k.second / (nwsum_local[k.first] + Vbeta);
				// rsum += temp;
				// q1[k.first] += temp;
			// }
			// rsum *= beta;

			// for (int n = 0; n < ptrndata->docs[m]->length; ++n)
			// {
				// int w = ptrndata->docs[m]->words[n];

				// // remove z_ij from the count variables
				// int topic = z[m][n]; int old_topic = topic;
				// nd_m[topic] -= 1;
				// nds1[m][rev_mapper[topic]].second -= 1;
				// nwsum_local[topic] -= 1;

				// // update the bucket sums
				// double denom = nwsum_local[topic] + Vbeta;
				// ssum -= (alpha * beta) / (denom + 1);
				// ssum += (alpha * beta) / denom;
				// rsum -= (beta + nd_m[topic] * beta) / (denom + 1);
				// rsum += (nd_m[topic] * beta) / denom;
				// q1[topic] = (alpha + nd_m[topic]) / denom;

				// ////  Divide the full sampling mass into three “buckets.”[s | r | q]
				// qsum = 0;
				// for (unsigned k = 0; k < nws[w].size(); ++k)
				// {
					// if (topic == nws[w][k].second)
						// p[k] = (nws[w][k].first - 1)*q1[nws[w][k].second];
					// else
						// p[k] = nws[w][k].first*q1[nws[w][k].second];
					// qsum += p[k];
				// }
				// double total_mass = qsum + rsum + ssum;
				// double u = d_unif01(urng) * total_mass;

				// if (u < ssum)	//“smoothing only” bucket
				// {
					// // In this case, we can step through each topic, calculating and adding up	for that topic, until we reach a value greater than u
					// u /= alpha * beta;
					// for (topic = -1; u > 0; u -= 1 / (nwsum_local[++topic] + Vbeta));
				// }
				// else if (u < ssum + rsum) //“document topic” bucket
				// {
					// // In this case, we need only iterate over the set of topics t such that ntd=0 a number that is usually substantially less than the total number of topics
					// u -= ssum;
					// u /= beta;
					// for (const auto& k : nds1[m])
					// {
						// u -= k.second / (nwsum_local[k.first] + Vbeta);
						// if (u <= 0)
						// {
							// topic = k.first;
							// break;
						// }
					// }

				// }
				// else //“topic word” bucket
				// {
					// u -= ssum + rsum;
					// for (topic = -1; u > 0; u -= p[++topic] );
					// topic = nws[w][topic].second;	
				// }
			
				// //update the bucket sums
				// denom = (nwsum_local[topic] + Vbeta);
				// ssum -= (alpha * beta) / denom;
				// ssum += (alpha * beta) / (denom + 1);
				// rsum -= (nd_m[topic] * beta) / denom;
				// rsum += (beta + nd_m[topic] * beta) / (denom + 1);
				// q1[topic] = (alpha + nd_m[topic] + 1) / (denom + 1);

				// // add newly estimated z_i to count variables
				// if (topic!=old_topic)
				// {
					// if(nd_m[topic] == 0)
					// {
						// rev_mapper[topic] = nds1[m].size();
						// nds1[m].push_back(std::pair<int, int>(topic, 1));
					// }
					// else
					// {
						// nds1[m][rev_mapper[topic]].second += 1;
					// }
					// nd_m[topic] += 1;
					// if (nd_m[old_topic] == 0)
					// {
						// nds1[m][rev_mapper[old_topic]].first = nds1[m].back().first;
						// nds1[m][rev_mapper[old_topic]].second = nds1[m].back().second;
						// rev_mapper[nds1[m].back().first] = rev_mapper[old_topic];
						// nds1[m].pop_back();
						// rev_mapper[old_topic] = -1;
					// }
						
					// cbuff[nst*(w%ntt)+i].push(delta(w,old_topic,topic));
				// }
				// else
				// {
					// nds1[m][rev_mapper[topic]].second += 1;
					// nd_m[topic] += 1;
				// }
				// nwsum_local[topic] += 1;
				// z[m][n] = topic;
			// }
			// for (const auto& k : nds1[m])
			// {
				// nd_m[k.first] = 0;
				// rev_mapper[k.first] = -1;
				// q1[k.first] -= k.second / (nwsum[k.first] + Vbeta);
			// }
		// }
		// tn = std::chrono::high_resolution_clock::now();
		// std::cout << "In thread " << i << " at iteration " << iter << " ..." 
				  // << "Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(tn - ts).count() << std::endl;
	// }
	
	// delete[] p;
	// delete[] nd_m;
	// delete[] rev_mapper;
	// return 0;
// }

int sparseLDA::sampling(int i)
{
	std::mt19937 urng(i);
	std::uniform_real_distribution<double> d_unif01(0.0, 1.0);
	
	double * p = new double[K]; // temp variable for sampling
	int *nd_m = new int[K];
	int *rev_mapper = new int[K];
	for (int k = 0; k < K; ++k)
	{
		nd_m[k] = 0;
		rev_mapper[k] = -1;
	}

	int *nwsum_old = new int[K];
	int *nwsum_local = new int[K];

	t_mtx.lock();
	for (int k = 0; k < K; ++k)
	{
		nwsum_old[k] = nwsum[k];
		nwsum_local[k] = nwsum[k];
	}
	t_mtx.unlock();

	double ssum, rsum, qsum;
	double *q1 = new double[K];
	// compute ssum
	ssum = 0;
	for (int k = 0; k < K; ++k)
	{
		q1[k] = alpha / (nwsum_local[k] + Vbeta);
		ssum += 1 / (nwsum_local[k] + Vbeta);
	}
	ssum *= alpha * beta;

	for (int liter = 0; liter < niters; ++liter)
	{
		std::cout << "In thread " << i << " at iteration " << liter << " ..." << std::endl;
		// for all z_ij
		for (int m = i*M / nst; m < (i + 1)*M / nst; ++m)
		{
			//synchronization
			if (m % 100 == 0)
			{
				t_mtx.lock();
				for (int k = 0; k < K; ++k)
				{
					nwsum[k] += nwsum_local[k] - nwsum_old[k];
					nwsum_old[k] = nwsum[k];
					nwsum_local[k] = nwsum[k];
				}
				t_mtx.unlock();
			}

			rsum = 0;
			int kc = 0;
			for (auto k = nds1[m].begin(); k != nds1[m].end(); ++k)
			{
				nd_m[k->first] = k->second;
				rev_mapper[k->first] = kc++;
				double temp = k->second / (nwsum_local[k->first] + Vbeta);
				rsum += temp;
				q1[k->first] += temp;
			}
			rsum *= beta;

			for (int n = 0; n < ptrndata->docs[m]->length; ++n)
			{
				// sample from p(z_i|z_-i, w)
				int topic = z[m][n]; int old_topic = topic;
				int w = ptrndata->docs[m]->words[n];
				mtx[w].lock();

				// remove z_ij from the count variables
				nw[w][topic] -= 1;
				nd_m[topic] -= 1;
				nds1[m][rev_mapper[topic]].second -= 1;
				nwsum_local[topic] -= 1;

				// update the bucket sums
				double denom = nwsum_local[topic] + Vbeta;
				ssum -= (alpha * beta) / (denom + 1);
				ssum += (alpha * beta) / denom;
				rsum -= (beta + nd_m[topic] * beta) / (denom + 1);
				rsum += (nd_m[topic] * beta) / denom;
				q1[topic] = (alpha + nd_m[topic]) / denom;

				////  Divide the full sampling mass into three “buckets.”[s | r | q]
				qsum = 0;
				for (unsigned k = 0; k < nws[w].size(); ++k)
				{
					if (topic == nws[w][k].second)
						p[k] = (nws[w][k].first - 1)*q1[nws[w][k].second];
					else
						p[k] = nws[w][k].first*q1[nws[w][k].second];
					qsum += p[k];
				}
				double total_mass = qsum + rsum + ssum;
				double u = d_unif01(urng) * total_mass;

				if (u < ssum)	//“smoothing only” bucket
				{
					// In this case, we can step through each topic, calculating and adding up	for that topic, until we reach a value greater than u
					u /= alpha * beta;
					for (topic = -1; u > 0; u -= 1. / (nwsum_local[++topic] + Vbeta));
				}
				else if (u < ssum + rsum) //“document topic” bucket
				{
					// In this case, we need only iterate over the set of topics t such that ntd=0 a number that is usually substantially less than the total number of topics
					u -= ssum;
					u /= beta;
					for (auto k = nds1[m].begin(); k != nds1[m].end(); ++k)
					{
						u -= k->second / (nwsum_local[k->first] + Vbeta);
						if (u <= 0)
						{
							topic = k->first;
							break;
						}
					}

				}
				else //“topic word” bucket
				{
					u -= ssum + rsum;
					for (topic = -1; u > 0; u -= p[++topic]);
					topic = nws[w][topic].second;
				}

				if (old_topic != topic) //maintaining the correct encoding of nw
				{
					/* Decrement after which it is sorted */
					unsigned k = 0;
					for (k = 0; k < nws[w].size(); ++k)
					{
						if (old_topic == nws[w][k].second)
						{
							nws[w][k].first -= 1;
							break;
						}
					}
					for (++k; k < nws[w].size(); ++k)
					{
						if (nws[w][k - 1].first >= nws[w][k].first)
							break;
						else
						{
							auto copy_temp = nws[w][k - 1];
							nws[w][k - 1] = nws[w][k];
							nws[w][k] = copy_temp;
						}
					}

					/* Remove zeros */
					while (nws[w].size() && nws[w].back().first == 0)	// remove zeros if any
					{
						nws[w].pop_back();
						nws[w].shrink_to_fit();
					}

					/* increment after which it is sorted as well */
					for (k = 0; k < nws[w].size(); ++k)
					{
						if (topic == nws[w][k].second)
						{
							nws[w][k].first += 1;
							break;
						}
					}
					if (k >= nws[w].size())	// if not found in existing topics
					{
						nws[w].push_back(std::pair<int, int>(1, topic));
					}
					else
					{
						for (; k > 0; --k)
						{
							if (nws[w][k - 1].first >= nws[w][k].first)
								break;
							else
							{
								auto copy_temp = nws[w][k - 1];
								nws[w][k - 1] = nws[w][k];
								nws[w][k] = copy_temp;
							}
						}
					}
				}

				//update the bucket sums
				denom = (nwsum_local[topic] + Vbeta);
				ssum -= (alpha * beta) / denom;
				ssum += (alpha * beta) / (denom + 1);
				rsum -= (nd_m[topic] * beta) / denom;
				rsum += (beta + nd_m[topic] * beta) / (denom + 1);
				q1[topic] = (alpha + nd_m[topic] + 1) / (denom + 1);

				// add newly estimated z_i to count variables
				if (old_topic != topic && nd_m[topic] == 0)
				{
					rev_mapper[topic] = nds1[m].size();
					nds1[m].push_back(std::pair<int, int>(topic, 1));
				}
				else
					nds1[m][rev_mapper[topic]].second += 1;
					nd_m[topic] += 1;
				if (nd_m[old_topic] == 0)
				{
					nds1[m][rev_mapper[old_topic]].first = nds1[m].back().first;
					nds1[m][rev_mapper[old_topic]].second = nds1[m].back().second;
					rev_mapper[nds1[m].back().first] = rev_mapper[old_topic];
					nds1[m].pop_back();
					rev_mapper[old_topic] = -1;
				}
				nwsum_local[topic] += 1;
				nw[w][topic] += 1;

				mtx[w].unlock();

				z[m][n] = topic;
			}

			for (auto k = nds1[m].begin(); k != nds1[m].end(); ++k)
			{
				nd_m[k->first] = 0;
				rev_mapper[k->first] = -1;
			}
		}
	}
	return 0;

}

int sparseLDA::updater(int i)
{
	do
	{
		for(int tn = 0; tn<nst; ++tn)
		{
			if(!cbuff[i*nst + tn].empty())
			{
				delta temp = cbuff[i*nst + tn].front();
				cbuff[i*nst + tn].pop();
				nw[temp.word][temp.old_topic] -= 1;
				nw[temp.word][temp.new_topic] += 1;
				nwsum[temp.old_topic] -= -1;
				nwsum[temp.new_topic] += +1;
				//nwsum[temp.old_topic].fetch_add(-1);
				//nwsum[temp.new_topic].fetch_add(+1);
				
				
				//maintaining the correct encoding of nws
				/* Decrement after which it is sorted */
				unsigned k = 0;
				for (k = 0; k < nws[temp.word].size(); ++k)
				{
					if (temp.old_topic == nws[temp.word][k].second)
					{
						nws[temp.word][k].first -= 1;
						break;
					}
				}
				for (++k; k < nws[temp.word].size(); ++k)
				{
					if (nws[temp.word][k-1].first >= nws[temp.word][k].first)
						break;
					else
					{
						auto copy_temp = nws[temp.word][k - 1];
						nws[temp.word][k - 1] = nws[temp.word][k];
						nws[temp.word][k] = copy_temp;
					}
				}
			
				/* Remove zeros */
				while (nws[temp.word].size() && nws[temp.word].back().first == 0)	// remove zeros if any
				{
					nws[temp.word].pop_back();
					nws[temp.word].shrink_to_fit();
				}

				/* increment after which it is sorted as well */
				for (k = 0; k < nws[temp.word].size(); ++k)
				{
					if (temp.new_topic == nws[temp.word][k].second)
					{
						nws[temp.word][k].first += 1;
						break;
					}
				}
				if (k >= nws[temp.word].size())	// if not found in existing topics
				{
					nws[temp.word].push_back(std::pair<int, int>(1, temp.new_topic));
				}
				else
				{
					for (; k > 0; --k)
					{
						if (nws[temp.word][k - 1].first >= nws[temp.word][k].first)
							break;
						else
						{
							auto copy_temp = nws[temp.word][k - 1];
							nws[temp.word][k - 1] = nws[temp.word][k];
							nws[temp.word][k] = copy_temp;
						}
					}
				}
			}
		}
	} while(!done[i]);

	return 0;
}

int aliasLDA::specific_init()
{
	std::cout << "Initializing the alias tables ..." << std::endl;
	q.resize(V);
	for (int w = 0; w < V; ++w)
	{
		q[w].init(K);
		generateQtable(w);
	}
	
	return 0;
}

int aliasLDA::sampling(int i)
{
	std::mt19937 urng(i);
	std::uniform_real_distribution<double> d_unif01(0.0, 1.0);
	
	double * p = new double[K]; // temp variable for sampling
	int *nd_m = new int[K];
	int *rev_mapper = new int[K];
	for (int k = 0; k < K; ++k)
	{
		nd_m[k] = 0;
		rev_mapper[k] = -1;
	}
	std::chrono::high_resolution_clock::time_point ts, tn;

	for (int iter = 1; iter <= niters; ++iter)
	{
		ts = std::chrono::high_resolution_clock::now();
		// for each document of worker i
		for (int m = i; m < M; m+=nst)
		{
			int kc = 0;
			for (const auto& k : nds1[m])
			{
				nd_m[k.first] = k.second;
				rev_mapper[k.first] = kc++;
			}
			for (int n = 0; n < ptrndata->docs[m]->length; ++n)
			{
				int w = ptrndata->docs[m]->words[n];
				
				// remove z_ij from the count variables
				int topic = z[m][n]; int new_topic; int old_topic = topic;
				nd_m[topic] -= 1;
				nds1[m][rev_mapper[topic]].second -= 1;

				//Compute pdw
				double psum = 0;
				int ii = 0;
				/* Travese all non-zero document-topic distribution */
				for (const auto& k : nds1[m])
				{
					//if (topic == k.first)
					//	psum += k.second * (nw[w][k.first] - 1 + beta) / (nwsum[k.first] - 1 + Vbeta);
					//else
						psum += k.second * (nw[w][k.first] + beta) / (nwsum[k.first] + Vbeta);
					p[ii++] = psum;
				}

				double select_pr = psum / (psum + alpha*q[w].wsum);

				//MHV to draw new topic
				for (int r = 0; r < MH_STEPS; ++r)
				{
					//1. Flip a coin
					if (d_unif01(urng) < select_pr)
					{
						double u = d_unif01(urng) * psum;
						new_topic = std::lower_bound(p,p+ii,u) - p;
						new_topic = nds1[m][new_topic].first;
					}
					else
					{
						q[w].noSamples++;
						if(q[w].noSamples > K>>1 )
						{	
							if(mtx[w].try_lock())
							{
								generateQtable(w);
								mtx[w].unlock();
							}
						}
						new_topic = q[w].sample(d_unif01(urng), d_unif01(urng));
					}
					
					_mm_prefetch((const char *)&(q[w].w[new_topic]), _MM_HINT_T1);
					_mm_prefetch((const char *)&(q[w].w[topic]), _MM_HINT_T1);

					if (topic != new_topic)
					{
						//2. Find acceptance probability
						double temp_old = (nw[w][topic] + beta) / (nwsum[topic] + Vbeta);
						double temp_new = (nw[w][new_topic] + beta) / (nwsum[new_topic] + Vbeta);
						//double temp_old = (old_topic!=topic) ? (nw[w][topic] + beta) / (nwsum[topic] + Vbeta) : (nw[w][topic] - 1 + beta) / (nwsum[topic] - 1 + Vbeta);
						//double temp_new = (old_topic!=new_topic) ? (nw[w][new_topic] + beta) / (nwsum[new_topic] + Vbeta) : (nw[w][new_topic] - 1 + beta) / (nwsum[new_topic] - 1 + Vbeta);
						double acceptance = (nd_m[new_topic] + alpha) / (nd_m[topic] + alpha)
							*temp_new / temp_old
							*(nd_m[topic] * temp_old + alpha*q[w].w[topic])
							/ (nd_m[new_topic] * temp_new + alpha*q[w].w[new_topic]);
						//3. Compare against uniform[0,1]
						if (d_unif01(urng) < acceptance)
							topic = new_topic;
					}
				}		
			
				// add newly estimated z_i to count variables
				if (topic!=old_topic)
				{
					if(nd_m[topic] == 0)
					{
						rev_mapper[topic] = nds1[m].size();
						nds1[m].push_back(std::pair<int, int>(topic, 1));
					}
					else
					{
						nds1[m][rev_mapper[topic]].second += 1;
					}
					nd_m[topic] += 1;
					if (nd_m[old_topic] == 0)
					{
						nds1[m][rev_mapper[old_topic]].first = nds1[m].back().first;
						nds1[m][rev_mapper[old_topic]].second = nds1[m].back().second;
						rev_mapper[nds1[m].back().first] = rev_mapper[old_topic];
						nds1[m].pop_back();
						rev_mapper[old_topic] = -1;
					}
				
					cbuff[nst*(w%ntt)+i].push(delta(w,old_topic, topic));
				}
				else
				{
					nds1[m][rev_mapper[topic]].second += 1;
					nd_m[topic] += 1;
				}
				z[m][n] = topic;
			}
			for (const auto& k : nds1[m])
			{
				nd_m[k.first] = 0;
				rev_mapper[k.first] = -1;
			}
		}
		tn = std::chrono::high_resolution_clock::now();
		std::cout << "In thread " << i << " at iteration " << iter << " ..." 
				  << "Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(tn - ts).count() << std::endl;
	}
	
	delete[] p;
	delete[] nd_m;
	delete[] rev_mapper;
	
	return 0;
}

void aliasLDA::generateQtable(int w)
{
	//voseAlias temp;
	//temp.init(K);
	double wsum = 0.0;
	for (int k = 0; k < K; ++k)
	{
		q[w].w[k] = (nw[w][k] + beta) / (nwsum[k] + Vbeta);
		wsum += q[w].w[k];
	}
	q[w].wsum = wsum;
	q[w].constructTable();
}

int FTreeLDA::specific_init()
{
	// Construct trees here
	std::cout << "Initializing the Fenwich trees ..." << std::endl;
	trees = new fTree[V];
	{
		double *temp = new double[K];
		for (int w = 0; w < V; ++w)
		{
			for (int k = 0; k < K; ++k)
				temp[k] = (nw[w][k] + beta) / (nwsum[k] + Vbeta);
			trees[w].init(K);
			trees[w].recompute(temp);
		}
	}
	return 0;
}

int FTreeLDA::sampling(int i)
{
	std::mt19937 urng(i);
	std::uniform_real_distribution<double> d_unif01(0.0, 1.0);

	double * p = new double[K]; // temp variable for sampling
	int *nd_m = new int[K];
	int *rev_mapper = new int[K];
	for (int k = 0; k < K; ++k)
	{
		nd_m[k] = 0;
		rev_mapper[k] = -1;
	}
	std::chrono::high_resolution_clock::time_point ts, tn;
	
	for (int iter = 1; iter <= niters; ++iter)
	{
		ts = std::chrono::high_resolution_clock::now();
		// for each document of worker i
		for (int m = i; m < M; m+=nst)
		{
			int kc = 0;
			for (const auto& k : nds1[m])
			{
				nd_m[k.first] = k.second;
				rev_mapper[k.first] = kc++;
			}
			for (int n = 0; n < ptrndata->docs[m]->length; ++n)
			{
				int w = ptrndata->docs[m]->words[n];
				
				// remove z_ij from the count variables
				int topic = z[m][n]; int old_topic = topic;
				nd_m[topic] -= 1;
				nds1[m][rev_mapper[topic]].second -= 1;

				// Multi core approximation: do not update fTree[w] apriori
				// trees[w].update(topic, (nw[w][topic] + beta) / (nwsum[topic] + Vbeta));

				//Compute pdw
				double psum = 0;
				int ii = 0;
				/* Travese all non-zero document-topic distribution */
				for (const auto& k : nds1[m])
				{
					psum += k.second * trees[w].getComponent(k.first);
					p[ii++] = psum;
				}

				double u = d_unif01(urng) * (psum + alpha*trees[w].w[1]);

				if (u < psum)
				{
					int temp = std::lower_bound(p,p+ii,u) - p;
					topic = nds1[m][temp].first;
				}
				else
				{
					topic = trees[w].sample(d_unif01(urng));
				}

				// add newly estimated z_i to count variables
				if (topic!=old_topic)
				{
					if(nd_m[topic] == 0)
					{
						rev_mapper[topic] = nds1[m].size();
						nds1[m].push_back(std::pair<int, int>(topic, 1));
					}
					else
					{
						nds1[m][rev_mapper[topic]].second += 1;
					}
					nd_m[topic] += 1;
					if (nd_m[old_topic] == 0)
					{
						nds1[m][rev_mapper[old_topic]].first = nds1[m].back().first;
						nds1[m][rev_mapper[old_topic]].second = nds1[m].back().second;
						rev_mapper[nds1[m].back().first] = rev_mapper[old_topic];
						nds1[m].pop_back();
						rev_mapper[old_topic] = -1;
					}
				
					cbuff[nst*(w%ntt)+i].push(delta(w,old_topic,topic));
				}
				else
				{
					nds1[m][rev_mapper[topic]].second += 1;
					nd_m[topic] += 1;
				}
				z[m][n] = topic;
			}
			for (const auto& k : nds1[m])
			{
				nd_m[k.first] = 0;
				rev_mapper[k.first] = -1;
			}
		}
		tn = std::chrono::high_resolution_clock::now();
		std::cout << "In thread " << i << " at iteration " << iter << " ..." 
				  << "Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(tn - ts).count() << std::endl;
	}
	std::cout<<"Returning from "<<i<<std::endl;
	
	delete[] p;
	delete[] nd_m;
	delete[] rev_mapper;
	
	return 0;	
}
int FTreeLDA::updater(int i)
{
	do
	{
		for(int tn = 0; tn<nst; ++tn)
		{
			if(!cbuff[i*nst + tn].empty())
			{
				delta temp = cbuff[i*nst + tn].front();
				cbuff[i*nst + tn].pop();
				nw[temp.word][temp.old_topic] -= 1;
				nw[temp.word][temp.new_topic] += 1;
				nwsum[temp.old_topic] -= 1;
				nwsum[temp.new_topic] += 1;
				//nwsum[temp.old_topic].fetch_add(-1);
				//nwsum[temp.new_topic].fetch_add(+1);
				
				trees[temp.word].update(temp.old_topic, (nw[temp.word][temp.old_topic] + beta) / (nwsum[temp.old_topic] + Vbeta));
				trees[temp.word].update(temp.new_topic, (nw[temp.word][temp.new_topic] + beta) / (nwsum[temp.new_topic] + Vbeta));
			}
		}
	} while(!done[i]);//(done.test_and_set());

	return 0;
}

int forestLDA::specific_init()
{
	// Construct forest here
	std::cout << "Initializing the Vitter forests ..." << std::endl;
	q = new forestSample[V];
	{
		double *temp = new double[K];
		for (int w = 0; w < V; ++w)
		{
			for (int k = 0; k < K; ++k)
			{
				temp[k] = (nw[w][k] + beta) / (nwsum[k] + Vbeta);
			}
			q[w].preprocess(K, temp);
		}
	}
	return 0;
}

int forestLDA::sampling(int i)
{
	std::mt19937 urng(i);
	std::uniform_real_distribution<double> d_unif01(0.0, 1.0);
	
	double * p = new double[K]; // temp variable for sampling
	int *nd_m = new int[K];
	int *rev_mapper = new int[K];
	for (int k = 0; k < K; ++k)
	{
		nd_m[k] = 0;
		rev_mapper[k] = -1;
	}
	std::chrono::high_resolution_clock::time_point ts, tn;
	
	for (int iter = 1; iter <= niters; ++iter)
	{
		ts = std::chrono::high_resolution_clock::now();
		// for each document of worker i
		for (int m = i; m < M; m+=nst)
		{
			int kc = 0;
			for (const auto& k : nds1[m])
			{
				nd_m[k.first] = k.second;
				rev_mapper[k.first] = kc++;
			}
			for (int n = 0; n < ptrndata->docs[m]->length; ++n)
			{
				int w = ptrndata->docs[m]->words[n];
				
				// remove z_ij from the count variables
				int topic = z[m][n]; int old_topic = topic;
				nd_m[topic] -= 1;
				nds1[m][rev_mapper[topic]].second -= 1;
				
				//Multi core approximation: do not update vitter[w] apriori
				//q[w].update(topic, (nw[w][topic] + beta) / (nwsum[topic] + Vbeta));
				
				//Compute pdw
				double psum = 0;
				int ii = 0;
				/* Travese all non-zero document-topic distribution */
				for (auto k = nds1[m].begin(); k != nds1[m].end(); ++k)
				{
					psum += k->second * q[w].forest[0][k->first].weight;
					p[ii++] = psum;
				}

				double u = d_unif01(urng) * (psum + alpha*q[w].wsum);

				if (u < psum)
				{
					int temp = std::lower_bound(p, p+ii, u) - p;
					topic = nds1[m][temp].first;					
				}
				else
				{
					topic = q[w].generate();
				}
				
				// add newly estimated z_i to count variables
				if (topic!=old_topic)
				{
					if(nd_m[topic] == 0)
					{
						rev_mapper[topic] = nds1[m].size();
						nds1[m].push_back(std::pair<int, int>(topic, 1));
					}
					else
					{
						nds1[m][rev_mapper[topic]].second += 1;
					}
					nd_m[topic] += 1;
					if (nd_m[old_topic] == 0)
					{
						nds1[m][rev_mapper[old_topic]].first = nds1[m].back().first;
						nds1[m][rev_mapper[old_topic]].second = nds1[m].back().second;
						rev_mapper[nds1[m].back().first] = rev_mapper[old_topic];
						nds1[m].pop_back();
						rev_mapper[old_topic] = -1;
					}
				
					cbuff[nst*(w%ntt)+i].push(delta(w,old_topic,topic));
				}
				else
				{
					nds1[m][rev_mapper[topic]].second += 1;
					nd_m[topic] += 1;
				}
				z[m][n] = topic;
			}
			for (const auto& k : nds1[m])
			{
				nd_m[k.first] = 0;
				rev_mapper[k.first] = -1;
			}
		}
		tn = std::chrono::high_resolution_clock::now();
		std::cout << "In thread " << i << " at iteration " << iter << " ..." 
				  << "Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(tn - ts).count() << std::endl;
	}
	
	delete[] p;
	delete[] nd_m;
	delete[] rev_mapper;
	return 0;
}

int forestLDA::updater(int i)
{
	do
	{
		for(int tn = 0; tn<nst; ++tn)
		{
			if(!cbuff[i*nst + tn].empty())
			{
				delta temp = cbuff[i*nst + tn].front();
				cbuff[i*nst + tn].pop();
	//			nw[temp.word][temp.old_topic] -= 1;
	//			nw[temp.word][temp.new_topic] += 1;
	//			nwsum[temp.old_topic] -= 1;
	//			nwsum[temp.new_topic] += 1;
				//nwsum[temp.old_topic].fetch_add(-1);
				//nwsum[temp.new_topic].fetch_add(+1);
				
	//			q[temp.word].update(temp.old_topic, (nw[temp.word][temp.old_topic] + beta) / (nwsum[temp.old_topic] + Vbeta) );
	//			q[temp.word].update(temp.new_topic, (nw[temp.word][temp.new_topic] + beta) / (nwsum[temp.new_topic] + Vbeta) );
			}
		}
	} while(!done[i]);

	return 0;
}

int lightLDA::specific_init()
{
	std::cout << "Initializing the alias tables ..." << std::endl;
	q.resize(V);
	for (int w = 0; w < V; ++w)
	{
		q[w].init(K);
		generateQtable(w);
	}
	return 0;
}

int lightLDA::sampling(int i)
{
	std::mt19937 urng(i);
	std::uniform_real_distribution<double> d_unif01(0.0, 1.0);
	
	double * p = new double[K]; // temp variable for sampling
	int *nd_m = new int[K];
	int *rev_mapper = new int[K];
	for (int k = 0; k < K; ++k)
	{
		nd_m[k] = 0;
		rev_mapper[k] = -1;
	}
	std::chrono::high_resolution_clock::time_point ts, tn;
	
	for (int iter = 1; iter <= niters; ++iter)
	{
		ts = std::chrono::high_resolution_clock::now();
		// for each document of worker i
		for (int m = i; m < M; m+=nst)
		{
			int kc = 0;
			for (const auto& k : nds1[m])
			//for (auto k = nds1[m].begin(); k != nds1[m].end(); ++k)
			{
				nd_m[k.first] = k.second;
				rev_mapper[k.first] = kc++;
			}
			
			double sumPd = ptrndata->docs[m]->length + K * alpha;
			
			for (int n = 0; n < ptrndata->docs[m]->length; ++n)
			{
				int w = ptrndata->docs[m]->words[n];
				
				// remove z_ij from the count variables
				int topic = z[m][n]; int new_topic; int old_topic = topic;
				nd_m[topic] -= 1;
				nds1[m][rev_mapper[topic]].second -= 1;

				//if(mtx[w].try_lock()){

				// MHV to draw new topic
				for (int r = 0; r < MH_STEPS; ++r)
				{
					{
						// Draw a topic from doc-proposal
						double u = d_unif01(urng) * sumPd;
						if (u < ptrndata->docs[m]->length)
						{
							// draw from doc-topic distribution skipping n
							int pos = (int)(u);
							new_topic = z[m][pos];
						}
						else
						{
							// draw uniformly
							u -= ptrndata->docs[m]->length;
							u /= alpha;
							new_topic = (int)(u); // pick_a_number(0,ptrndata->docs[m]->length-1); (int)(d_unif01(urng)*ptrndata->docs[m]->length);
						}

						if (topic != new_topic)
						{
							//2. Find acceptance probability
							double temp_old = (nd_m[topic] + alpha) * (nw[w][topic] + beta) / (nwsum[topic] + Vbeta) ;
							double temp_new = (nd_m[new_topic] + alpha) * (nw[w][new_topic] + beta) / (nwsum[new_topic] + Vbeta);
							double prop_old = (topic==old_topic) ? (nd_m[topic] + 1 + alpha) : (nd_m[topic] + alpha);
							double prop_new = (new_topic==old_topic) ? (nd_m[new_topic] + 1 + alpha) : (nd_m[new_topic] + alpha);
							double acceptance = (temp_new * prop_old) / (temp_old *prop_new);
								
							//3. Compare against uniform[0,1]
							if (d_unif01(urng) < acceptance)
							{
								topic = new_topic;
							}
						}
					}
				
					{
						// Draw a topic from word-proposal
						q[w].noSamples++;
						if (q[w].noSamples > K >> 1)
						{
							if(mtx[w].try_lock())
							{
								generateQtable(w);
								mtx[w].unlock();
							}
						}
						new_topic = q[w].sample(d_unif01(urng),d_unif01(urng) );

					
						if (topic != new_topic)
						{
							//2. Find acceptance probability
							double temp_old = (nd_m[topic] + alpha) * (nw[w][topic] + beta) / (nwsum[topic] + Vbeta);
							double temp_new = (nd_m[new_topic] + alpha) * (nw[w][new_topic] + beta) / (nwsum[new_topic] + Vbeta);
							double acceptance =  (temp_new * q[w].w[topic]) / (temp_old * q[w].w[new_topic]);
					
							//3. Compare against uniform[0,1]
							if (d_unif01(urng) < acceptance)
							{
								topic = new_topic;
							}
						}
					}
				}

				// add newly estimated z_i to count variables
				if (topic!=old_topic)
				{
					if(nd_m[topic] == 0)
					{
						rev_mapper[topic] = nds1[m].size();
						nds1[m].push_back(std::pair<int, int>(topic, 1));
					}
					else
					{
						nds1[m][rev_mapper[topic]].second += 1;
					}
					nd_m[topic] += 1;
					if (nd_m[old_topic] == 0)
					{
						nds1[m][rev_mapper[old_topic]].first = nds1[m].back().first;
						nds1[m][rev_mapper[old_topic]].second = nds1[m].back().second;
						rev_mapper[nds1[m].back().first] = rev_mapper[old_topic];
						nds1[m].pop_back();
						rev_mapper[old_topic] = -1;
					}
				
					cbuff[nst*(w%ntt)+i].push(delta(w,old_topic,topic));
				}
				else
				{
					nds1[m][rev_mapper[topic]].second += 1;
					nd_m[topic] += 1;
				}
				z[m][n] = topic;
			}
			for (const auto& k : nds1[m])
			{
				nd_m[k.first] = 0;
				rev_mapper[k.first] = -1;
			}
		}
		tn = std::chrono::high_resolution_clock::now();
		std::cout << "In thread " << i << " at iteration " << iter << " ..." 
				  << "Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(tn - ts).count() << std::endl;
	}
	
	delete[] p;
	delete[] nd_m;
	delete[] rev_mapper;
	return 0;
}

void lightLDA::generateQtable(int w)
{
	//voseAlias temp;
	//temp.init(K);
	double wsum = 0.0;
	for (int k = 0; k < K; ++k)
	{
		q[w].w[k] = (nw[w][k] + beta) / (nwsum[k] + Vbeta);
		wsum += q[w].w[k];
	}
	q[w].wsum = wsum;
	q[w].constructTable();
}
