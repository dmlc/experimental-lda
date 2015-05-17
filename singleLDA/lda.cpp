#include "lda.h"
#include <mmintrin.h>

int simpleLDA::specific_init()
{
	return 0;
}

int simpleLDA::sampling(int m)
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
		remove_from_topic( w, m, topic);

		// do multinomial sampling via cumulative method
		double temp = 0;
		for (int k = 0; k < K; k++)
		{
			temp += (nw[w][k] + beta) / (nwsum[k] + Vbeta) *
				(nd_m[k] + alpha);
			p[k] = temp;
		}
		
		// scaled sample because of unnormalized p[]
		double u = utils::unif01() * temp;

		// Do a binary search instead!
		topic = utils::binary_search(p, u, 0, K - 1);
		
		// add newly estimated z_i to count variables
		add_to_topic( w, m, topic, old_topic );
		nd_m[topic] += 1;
		z[m][n] = topic;
	}
	for (const auto& k : nds1[m])
	{
		nd_m[k.first] = 0;
		rev_mapper[k.first] = -1;
	}
	return 0;
}

int unifLDA::specific_init()
{
	return 0;
}

int unifLDA::sampling(int m)
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
		remove_from_topic( w, m, topic);

		// MH to draw new topic
		for (int r = 0; r < MH_STEPS; ++r)
		{
			// 1. Draw from the uniform proposal
			new_topic = (int)(utils::unif01()*K);
			
			//2. Find acceptance probability
			double temp_old = (nd_m[topic] + alpha) * (nw[w][topic] + beta) / (nwsum[topic] + Vbeta);
			double temp_new = (nd_m[new_topic] + alpha) * (nw[w][new_topic] + beta) / (nwsum[new_topic] + Vbeta);
			double acceptance =  (temp_new) / (temp_old);
			
			//3. Compare against uniform[0,1]
			if (utils::unif01() < acceptance)
			{
				topic = new_topic;
			}
		}

		// add newly estimated z_i to count variables
		add_to_topic( w, m, topic, old_topic );
		nd_m[topic] += 1;
		z[m][n] = topic;
	}
	for (const auto& k : nds1[m])
	{
		nd_m[k.first] = 0;
		rev_mapper[k.first] = -1;
	}
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
	
	q1 = new double[K];
	// compute ssum
	ssum = 0;
	for (int k = 0; k < K; ++k)
	{
		q1[k] = alpha / (nwsum[k] + Vbeta);
		ssum += 1 / (nwsum[k] + Vbeta);
	}
	ssum *= alpha * beta;		
	return 0;
}

int sparseLDA::sampling(int m)
{
	int kc = 0;
	rsum = 0;
	for (const auto& k : nds1[m])
	{
		nd_m[k.first] = k.second;
		rev_mapper[k.first] = kc++;
		double temp = k.second / (nwsum[k.first] + Vbeta);
		rsum += temp;
		q1[k.first] += temp;
	}
	rsum *= beta;

	for (int n = 0; n < ptrndata->docs[m]->length; ++n)
	{
		int w = ptrndata->docs[m]->words[n];

		// remove z_ij from the count variables
		int topic = z[m][n]; int old_topic = topic;
		remove_from_topic( w, m, topic);

		// update the bucket sums
		double denom = nwsum[topic] + Vbeta;
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
		double u = utils::unif01() * total_mass;

		if (u < ssum)	//“smoothing only” bucket
		{
			// In this case, we can step through each topic, calculating and adding up	for that topic, until we reach a value greater than u
			u /= alpha * beta;
			for (topic = -1; u > 0; u -= 1 / (nwsum[++topic] + Vbeta));
		}
		else if (u < ssum + rsum) //“document topic” bucket
		{
			// In this case, we need only iterate over the set of topics t such that ntd=0 a number that is usually substantially less than the total number of topics
			u -= ssum;
			u /= beta;
			for (const auto& k : nds1[m])
			{
				u -= k.second / (nwsum[k.first] + Vbeta);
				if (u <= 0)
				{
					topic = k.first;
					break;
				}
			}

		}
		else //“topic word” bucket
		{
			u -= ssum + rsum;
			for (topic = -1; u > 0; u -= p[++topic] );
			topic = nws[w][topic].second;	
		}

		if (topic!=old_topic) //maintaining the correct encoding of nw
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
				if (nws[w][k-1].first >= nws[w][k].first)
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
		denom = (nwsum[topic] + Vbeta);
		ssum -= (alpha * beta) / denom;
		ssum += (alpha * beta) / (denom + 1);
		rsum -= (nd_m[topic] * beta) / denom;
		rsum += (beta + nd_m[topic] * beta) / (denom + 1);
		q1[topic] = (alpha + nd_m[topic] + 1) / (denom + 1);

		// add newly estimated z_i to count variables
		add_to_topic( w, m, topic, old_topic );

		z[m][n] = topic;
	}
	for (const auto& k : nds1[m])
	{
		nd_m[k.first] = 0;
		rev_mapper[k.first] = -1;
		q1[k.first] -= k.second / (nwsum[k.first] + Vbeta);
	}
	return 0;
}

int aliasLDA::specific_init()
{
	std::cout << "Initializing the alias tables ..." << std::endl;
	q.resize(V);
	for (int w = 0; w < V; ++w)
	{
		q[w] = new (std::nothrow) voseAlias;
		q[w]->init(K);
		generateQtable(w);
	}
	
	return 0;
}

int aliasLDA::sampling(int m)
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
		remove_from_topic( w, m, topic);

		//Compute pdw
		double psum = 0;
		int i = 0;
		/* Travese all non-zero document-topic distribution */
		for (const auto& k : nds1[m])
		{
			psum += k.second * (nw[w][k.first] + beta) / (nwsum[k.first] + Vbeta);
			p[i++] = psum;
		}

		//mtx[w].lock();
		//if(mtx[w].try_lock())
		{

			double select_pr = psum / (psum + alpha*q[w]->wsum);

			//MHV to draw new topic
			for (int r = 0; r < MH_STEPS; ++r)
			{
				//1. Flip a coin
				if (utils::unif01() < select_pr)
				{
					double u = utils::unif01() * psum;
					new_topic = std::lower_bound(p,p+i,u) - p;
					//new_topic = utils::binary_search(p, u, 0, i);
					//for (new_topic = -1; u >0; u -= p[++new_topic]);
					new_topic = nds1[m][new_topic].first;
				}
				else
				{
					//mtx[w].lock();
					//mtx[w].unlock();	
					q[w]->noSamples++;
					if(q[w]->noSamples > K>>1 )
					{	
						generateQtable(w);
						//tu->enqueue(&aliasLDA::generateQtable, this, w);
						//table_updaters->enqueue(&aliasLDA::generateQtable, this, w);
					}
					new_topic = q[w]->sample();
				}

				_mm_prefetch((const char *)&(q[w]->w[topic]), _MM_HINT_T1);

				if (topic != new_topic)
				{
					//2. Find acceptance probability
					double temp_old = (nw[w][topic] + beta) / (nwsum[topic] + Vbeta);
					double temp_new = (nw[w][new_topic] + beta) / (nwsum[new_topic] + Vbeta);
					double acceptance = (nd_m[new_topic] + alpha) / (nd_m[topic] + alpha)
						*temp_new / temp_old
						*(nd_m[topic] * temp_old + alpha*q[w]->w[topic])
						/ (nd_m[new_topic] * temp_new + alpha*q[w]->w[new_topic]);


					//3. Compare against uniform[0,1]
					if (utils::unif01() < acceptance)
						topic = new_topic;
				}
			}

			//mtx[w].unlock();
			
		}
		
		// add newly estimated z_i to count variables
		add_to_topic( w, m, topic, old_topic );
		z[m][n] = topic;

	}
	for (const auto& k : nds1[m])
	{
		nd_m[k.first] = 0;
		rev_mapper[k.first] = -1;
	}
	return 0;
}

void aliasLDA::generateQtable(int w)
{
	//mtx[w].lock();
	//if(q[w]->noSamples > K>>3 || q[w]->noSamples < 0 )
	{	
		//voseAlias *temp = new (std::nothrow) voseAlias;
		//temp->init(K);
		q[w]->wsum = 0.0;
		for (int k = 0; k < K; ++k)
		{
			q[w]->w[k] = (nw[w][k] + beta) / (nwsum[k] + Vbeta);
			q[w]->wsum += q[w]->w[k];
		}
		q[w]->constructTable();
	
		//std::lock_guard<std::mutex> lock(mtx[w]);
	
		//voseAlias *swtmp = q[w];
		//q[w] = temp;
		//delete swtmp;
	}
	//mtx[w].unlock();
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

int FTreeLDA::sampling(int m)
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
		remove_from_topic( w, m, topic);

		// update fTree[w]
		trees[w].update(topic, (nw[w][topic] + beta) / (nwsum[topic] + Vbeta)
			- trees[w].getComponent(topic));

		//Compute pdw
		double psum = 0;
		int i = -1;
		/* Travese all non-zero document-topic distribution */
		//for (auto k = nds1[m].begin(); k != nds1[m].end(); ++k)
		for (const auto& k : nds1[m])
		{
			psum += k.second * (nw[w][k.first] + beta) / (nwsum[k.first] + Vbeta);
			p[++i] = psum;
		}

		double u = utils::unif01() * (psum + alpha*trees[w].w[1]);

		if (u < psum)
		{
			int temp = utils::binary_search(p, u, 0, i);
			topic = nds1[m][temp].first;
		}
		else
		{
			topic = trees[w].sample();
		}

		// add newly estimated z_i to count variables
		add_to_topic( w, m, topic, old_topic );

		// update fTree[w]
		trees[w].update(topic, (nw[w][topic] + beta) / (nwsum[topic] + Vbeta)
			- trees[w].getComponent(topic));

		z[m][n] = topic;
	}
	for (const auto& k : nds1[m])
	{
		nd_m[k.first] = 0;
		rev_mapper[k.first] = -1;
	}
	return 0;	
}

int forestLDA::specific_init()
{
	// Construct forest here
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

int forestLDA::sampling(int m)
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
		double copy_old_val = q[w].forest[0][topic].weight;
		remove_from_topic( w, m, topic);
		
		// update vitter[w]
		//q[w].update(topic, (nw[w][topic] + beta) / (nwsum[topic] + Vbeta));
		double temp_calc = (nw[w][topic] + beta) / (nwsum[topic] + Vbeta);
		q[w].forest[0][topic].weight = temp_calc;
		q[w].wsum += temp_calc - copy_old_val;

		//Compute pdw
		double psum = 0;
		int i = -1;
		/* Travese all non-zero document-topic distribution */
		for (auto k = nds1[m].begin(); k != nds1[m].end(); ++k)
		{
			psum += k->second * q[w].forest[0][k->first].weight; // (nw[w][k->first] + beta) / (nwsum[k->first] + Vbeta);
			p[++i] = psum;
		}

		double u = utils::unif01() * (psum + alpha*q[w].wsum);

		if (u < psum)
		{
			int temp = utils::binary_search(p, u, 0, i);
			topic = nds1[m][temp].first;

			// add newly estimated z_i to count variables
			add_to_topic( w, m, topic, old_topic );

			q[w].forest[0][old_topic].weight = copy_old_val;
			q[w].wsum += copy_old_val - temp_calc;
			if (topic != old_topic)
			{
				q[w].update(old_topic, temp_calc);
				temp_calc = (nw[w][topic] + beta) / (nwsum[topic] + Vbeta);
				q[w].update(topic, temp_calc);

			}
			
		}
		else
		{
			q[w].forest[0][old_topic].weight = copy_old_val;
			q[w].wsum += copy_old_val - temp_calc;
			q[w].update(old_topic, temp_calc);
			topic = q[w].generate();


			// add newly estimated z_i to count variables
			add_to_topic( w, m, topic, old_topic );

			temp_calc = (nw[w][topic] + beta) / (nwsum[topic] + Vbeta);
			q[w].update(topic, temp_calc);
		}
	
		z[m][n] = topic;
	}
	for (const auto& k : nds1[m])
	{
		nd_m[k.first] = 0;
		rev_mapper[k.first] = -1;
	}
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

int lightLDA::sampling(int m)
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
		remove_from_topic( w, m, topic);

		//if(mtx[w].try_lock()){

		// MHV to draw new topic
		for (int r = 0; r < MH_STEPS; ++r)
		{
			{
				// Draw a topic from doc-proposal
				double u = utils::unif01() * sumPd;
				if (u < ptrndata->docs[m]->length)
				{
					// draw from doc-topic distribution skipping n
					int pos = (int)(u); // utils::pick_a_number(0, ptrndata->docs[m]->length - 1);
					new_topic = z[m][pos];
				}
				else
				{
					// draw uniformly
					u -= ptrndata->docs[m]->length;
					u /= alpha;
					new_topic = (int)(u); // pick_a_number(0,ptrndata->docs[m]->length-1); (int)(utils::unif01()*ptrndata->docs[m]->length);
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
					if (utils::unif01() < acceptance)
					{
						topic = new_topic;
					}
				}
			}
		
			{
				// Draw a topic from word-proposal
				new_topic = q[w].sample();
				q[w].noSamples++;
				if(q[w].noSamples > K>>1 )
							{
									generateQtable(w);
									//tu->enqueue(&lightLDA::generateQtable, this, w);
									//table_updaters->enqueue(&lightLDA::generateQtable, this, w);
							}

			
				if (topic != new_topic)
				{
					//2. Find acceptance probability
					double temp_old = (nd_m[topic] + alpha) * (nw[w][topic] + beta) / (nwsum[topic] + Vbeta);
					double temp_new = (nd_m[new_topic] + alpha) * (nw[w][new_topic] + beta) / (nwsum[new_topic] + Vbeta);
					double acceptance =  (temp_new * q[w].w[topic]) / (temp_old * q[w].w[new_topic]);
			
					//3. Compare against uniform[0,1]
					if (utils::unif01() < acceptance)
					{
						topic = new_topic;
					}
				}
			}
			

		}

		//mtx[w].unlock();}

		// add newly estimated z_i to count variables
		add_to_topic( w, m, topic, old_topic );
		z[m][n] = topic;
	}
	for (const auto& k : nds1[m])
	{
		nd_m[k.first] = 0;
		rev_mapper[k.first] = -1;
	}
	return 0;
}

void lightLDA::generateQtable(int w)
{
//	mtx[w].lock();
	//if(q[w].noSamples > K>>2 || q[w].noSamples < 0 )
    {
		q[w].wsum = 0.0;
		for (int k = 0; k < K; ++k)
		{
			q[w].w[k] = (nw[w][k] + beta) / (nwsum[k] + Vbeta);
			q[w].wsum += q[w].w[k];
		}
		q[w].constructTable();
	}
//	mtx[w].unlock();
}
