#include "lda.h"
#include <mmintrin.h>

#define MH_STEPS 2

int simpleLDA::specific_init()
{
    return 0;
}

int simpleLDA::sampling(unsigned m)
{
    unsigned short kc = 0;
    for (const auto& k : n_mks[m])
    {
        nd_m[k.idx] = k.val;
        rev_mapper[k.idx] = kc++;
    }
    for (unsigned n = 0; n < trngdata->docs[m]->length; ++n)
    {
        unsigned w = trngdata->docs[m]->words[n];
		
        // remove z_ij from the count variables
        unsigned short topic = z[m][n]; unsigned short old_topic = topic;
        remove_from_topic( w, m, topic);

        // do multinomial sampling via cumulative method
        double temp = 0;
        for (unsigned short k = 0; k < K; k++)
        {
            temp += (nd_m[k] + alphaK) * (n_wk[w][k] + beta) / (n_k[k] + Vbeta);
            p[k] = temp;
        }

        // scaled sample because of unnormalized p[]
        double u = rng_.rand_double() * temp;

        // Do a binary search instead!
        topic = std::lower_bound(p, p+K, u) - p;

        // add newly estimated z_i to count variables
        add_to_topic( w, m, topic );
        z[m][n] = topic;
    }
    for (const auto& k : n_mks[m])
    {
        nd_m[k.idx] = 0;
        rev_mapper[k.idx] = K;
    }
    return 0;
}

int unifLDA::specific_init()
{
    return 0;
}

int unifLDA::sampling(unsigned m)
{
    unsigned short kc = 0;
    for (const auto& k : n_mks[m])
    {
        nd_m[k.idx] = k.val;
        rev_mapper[k.idx] = kc++;
    }
    for (unsigned n = 0; n < trngdata->docs[m]->length; ++n)
    {
        unsigned w = trngdata->docs[m]->words[n];

        // remove z_ij from the count variables
        unsigned short topic = z[m][n]; unsigned short new_topic; unsigned short old_topic = topic;
        remove_from_topic( w, m, topic);

        // MH to draw new topic
        for (unsigned r = 0; r < MH_STEPS; ++r)
        {
            // 1. Draw from the uniform proposal
            new_topic = rng_.rand_k(K);

            //2. Find acceptance probability
            double temp_old = (nd_m[topic] + alphaK) * (n_wk[w][topic] + beta) / (n_k[topic] + Vbeta);
            double temp_new = (nd_m[new_topic] + alphaK) * (n_wk[w][new_topic] + beta) / (n_k[new_topic] + Vbeta);
            double acceptance =  (temp_new) / (temp_old);

            //3. Compare against uniform[0,1]
            if (rng_.rand_double() < acceptance)
                topic = new_topic;
        }

        // add newly estimated z_i to count variables
        add_to_topic( w, m, topic );
        z[m][n] = topic;
    }
    for (const auto& k : n_mks[m])
    {
        nd_m[k.idx] = 0;
        rev_mapper[k.idx] = K;
    }
    return 0;
}


int sparseLDA::specific_init()
{
    std::cout << "Converting to sparseLDA data structure ..." << std::endl;
    nws.resize(V);
    for (unsigned v = 0; v < V; ++v)
    {
        for (unsigned short k = 0; k < K; ++k)
        {
            if (n_wk[v][k] != 0)
                nws[v].push_back(std::pair<unsigned, unsigned short >(n_wk[v][k],k));
        }
        std::sort(nws[v].begin(), nws[v].end(), std::greater<std::pair<unsigned, unsigned short>>());
    }
	
    q1 = new double[K];
    // compute ssum
    ssum = 0;
    for (unsigned short k = 0; k < K; ++k)
    {
        q1[k] = alphaK / (n_k[k] + Vbeta);
        ssum += 1 / (n_k[k] + Vbeta);
    }
    ssum *= alphaK * beta;		
    return 0;
}

int sparseLDA::sampling(unsigned m)
{
    unsigned short kc = 0;
    rsum = 0;
    for (const auto& k : n_mks[m])
    {
            nd_m[k.idx] = k.val;
            rev_mapper[k.idx] = kc++;
            double temp = k.val / (n_k[k.idx] + Vbeta);
            rsum += temp;
            q1[k.idx] += temp;
    }
    rsum *= beta;

    for (unsigned n = 0; n < trngdata->docs[m]->length; ++n)
    {
        unsigned w = trngdata->docs[m]->words[n];

        // remove z_ij from the count variables
        unsigned short topic = z[m][n]; unsigned short old_topic = topic;
        remove_from_topic( w, m, topic);

        // update the bucket sums
        double denom = n_k[topic] + Vbeta;
        ssum -= (alphaK * beta) / (denom + 1);
        ssum += (alphaK * beta) / denom;
        rsum -= (beta + nd_m[topic] * beta) / (denom + 1);
        rsum += (nd_m[topic] * beta) / denom;
        q1[topic] = (alphaK + nd_m[topic]) / denom;

        ////  Divide the full sampling mass into three �buckets.�[s | r | q]
        qsum = 0;
        for (unsigned short k = 0; k < nws[w].size(); ++k)
        {
            if (topic == nws[w][k].second)
                p[k] = (nws[w][k].first - 1)*q1[nws[w][k].second];
            else
                p[k] = nws[w][k].first*q1[nws[w][k].second];
            qsum += p[k];
        }
        double total_mass = qsum + rsum + ssum;
        double u = rng_.rand_double() * total_mass;

        if (u < ssum)	//�smoothing only� bucket
        {
            // In this case, we can step through each topic, calculating and adding up	for that topic, until we reach a value greater than u
            u /= alphaK * beta;
            for (topic = -1; u > 0; u -= 1 / (n_k[++topic] + Vbeta));
        }
        else if (u < ssum + rsum) //�document topic� bucket
        {
            // In this case, we need only iterate over the set of topics t such that ntd=0 a number that is usually substantially less than the total number of topics
            u -= ssum;
            u /= beta;
            for (const auto& k : n_mks[m])
            {
                u -= k.val / (n_k[k.idx] + Vbeta);
                if (u <= 0)
                {
                    topic = k.idx;
                    break;
                }
            }
        }
        else //�topic word� bucket
        {
                u -= ssum + rsum;
                for (topic = -1; u > 0; u -= p[++topic] );
                topic = nws[w][topic].second;	
        }

        if (topic!=old_topic) //maintaining the correct encoding of nw
        {
            /* Decrement after which it is sorted */
            unsigned short k = 0;
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
        denom = (n_k[topic] + Vbeta);
        ssum -= (alphaK * beta) / denom;
        ssum += (alphaK * beta) / (denom + 1);
        rsum -= (nd_m[topic] * beta) / denom;
        rsum += (beta + nd_m[topic] * beta) / (denom + 1);
        q1[topic] = (alphaK + nd_m[topic] + 1) / (denom + 1);

        // add newly estimated z_i to count variables
        add_to_topic( w, m, topic );

        z[m][n] = topic;
    }
    for (const auto& k : n_mks[m])
    {
        nd_m[k.idx] = 0;
        rev_mapper[k.idx] = K;
        q1[k.idx] -= k.val / (n_k[k.idx] + Vbeta);
    }
    return 0;
}

int aliasLDA::specific_init()
{
    std::cout << "Initializing the alias tables ..." << std::endl;
    q.resize(V);
    for (unsigned w = 0; w < V; ++w)
    {
        q[w].init(K);
        generateQtable(w);
    }

    return 0;
}

int aliasLDA::sampling(unsigned m)
{
    unsigned short kc = 0;
    for (const auto& k : n_mks[m])
    {
        nd_m[k.idx] = k.val;
        rev_mapper[k.idx] = kc++;
    }
    for (int n = 0; n < trngdata->docs[m]->length; ++n)
    {
        unsigned w = trngdata->docs[m]->words[n];

        // remove z_ij from the count variables
        unsigned short topic = z[m][n]; unsigned short new_topic; unsigned short old_topic = topic;
        remove_from_topic( w, m, topic);

        //Compute pdw
        double psum = 0;
        unsigned short ii = 0;
        /* Travese all non-zero document-topic distribution */
        for (const auto& k : n_mks[m])
        {
            psum += k.val * (n_wk[w][k.idx] + beta) / (n_k[k.idx] + Vbeta);
            p[ii++] = psum;
        }

        {
            double select_pr = psum / (psum + alphaK*q[w].wsum);

            //MHV to draw new topic
            for (int r = 0; r < MH_STEPS; ++r)
            {
                //1. Flip a coin
                if (utils::unif01() < select_pr)
                {
                    double u = rng_.rand_double() * psum;
                    new_topic = std::lower_bound(p,p+ii,u) - p;
                    new_topic = n_mks[m].idx_in(new_topic);
                }
                else
                {
                    q[w].noSamples++;
                    if(q[w].noSamples > K>>1 )
                    {	
                        generateQtable(w);
                    }
                    new_topic = q[w].sample(rng_.rand_k(K), rng_.rand_double());
                }

                _mm_prefetch((const char *)&(q[w].w[topic]), _MM_HINT_T1);

                if (topic != new_topic)
                {
                    //2. Find acceptance probability
                    double temp_old = (n_wk[w][topic] + beta) / (n_k[topic] + Vbeta);
                    double temp_new = (n_wk[w][new_topic] + beta) / (n_k[new_topic] + Vbeta);
                    double acceptance = (nd_m[new_topic] + alphaK) / (nd_m[topic] + alphaK)
                            *temp_new / temp_old
                            *(nd_m[topic] * temp_old + alphaK*q[w].w[topic])
                            / (nd_m[new_topic] * temp_new + alphaK*q[w].w[new_topic]);


                    //3. Compare against uniform[0,1]
                    if (rng_.rand_double() < acceptance)
                        topic = new_topic;
                }
            }
        }

        // add newly estimated z_i to count variables
        add_to_topic( w, m, topic );
        z[m][n] = topic;
    }
    for (const auto& k : n_mks[m])
    {
            nd_m[k.idx] = 0;
            rev_mapper[k.idx] = K;
    }
    return 0;
}

void aliasLDA::generateQtable(unsigned w)
{
    q[w].wsum = 0.0;
    for (unsigned short k = 0; k < K; ++k)
    {
        q[w].w[k] = (n_wk[w][k] + beta) / (n_k[k] + Vbeta);
        q[w].wsum += q[w].w[k];
    }
    q[w].constructTable();
}

int FTreeLDA::specific_init()
{
    // Construct trees here
    std::cout << "Initializing the Fenwich trees ..." << std::endl;
    trees = new fTree[V];
    {
        double *temp = new double[K];
        for (unsigned w = 0; w < V; ++w)
        {
            for (unsigned short k = 0; k < K; ++k)
                temp[k] = (n_wk[w][k] + beta) / (n_k[k] + Vbeta);
            trees[w].init(K);
            trees[w].recompute(temp);
        }
    }
    return 0;
}

int FTreeLDA::sampling(unsigned m)
{
    unsigned short kc = 0;
    for (const auto& k : n_mks[m])
    {
        nd_m[k.idx] = k.val;
        rev_mapper[k.idx] = kc++;
    }
    for (unsigned n = 0; n < trngdata->docs[m]->length; ++n)
    {
        unsigned w = trngdata->docs[m]->words[n];

        // remove z_ij from the count variables
        unsigned short topic = z[m][n]; unsigned short old_topic = topic;
        remove_from_topic( w, m, topic);

        // update fTree[w]
        trees[w].update(topic, (n_wk[w][topic] + beta) / (n_k[topic] + Vbeta));

        //Compute pdw
        double psum = 0;
        unsigned short ii = 0;
        /* Travese all non-zero document-topic distribution */
        for (const auto& k : n_mks[m])
        {
            psum += k.val * (n_wk[w][k.idx] + beta) / (n_k[k.idx] + Vbeta);
            p[ii++] = psum;
        }

            double u = rng_.rand_double() * (psum + alphaK*trees[w].w[1]);

            if (u < psum)
            {
                unsigned short temp = std::lower_bound(p, p+ii, u) - p;
                topic = n_mks[m].idx_in(temp);
            }
            else
            {
                topic = trees[w].sample(rng_.rand_double());
            }

            // add newly estimated z_i to count variables
            add_to_topic( w, m, topic );

            // update fTree[w]
            trees[w].update(topic, (n_wk[w][topic] + beta) / (n_k[topic] + Vbeta));

            z[m][n] = topic;
    }
    for (const auto& k : n_mks[m])
    {
        nd_m[k.idx] = 0;
        rev_mapper[k.idx] = K;
    }
    return 0;	
}

int forestLDA::specific_init()
{
    // Construct forest here
    q = new forestSample[V];
    {
        double *temp = new double[K];
        for (unsigned w = 0; w < V; ++w)
        {
            for (unsigned short k = 0; k < K; ++k)
            {
                temp[k] = (n_wk[w][k] + beta) / (n_k[k] + Vbeta);
            }
            q[w].preprocess(K, temp);
        }
    }
    return 0;
}

int forestLDA::sampling(unsigned m)
{
    unsigned short kc = 0;
    for (const auto& k : n_mks[m])
    {
        nd_m[k.idx] = k.idx;
        rev_mapper[k.idx] = kc++;
    }
    for (unsigned n = 0; n < trngdata->docs[m]->length; ++n)
    {
        unsigned w = trngdata->docs[m]->words[n];

        // remove z_ij from the count variables
        unsigned short topic = z[m][n]; unsigned short old_topic = topic;
        double copy_old_val = q[w].forest[0][topic].weight;
        remove_from_topic( w, m, topic);

        // update vitter[w]
        double temp_calc = (n_wk[w][topic] + beta) / (n_k[topic] + Vbeta);
        q[w].forest[0][topic].weight = temp_calc;
        q[w].wsum += temp_calc - copy_old_val;

        //Compute pdw
        double psum = 0;
        unsigned short ii = 0;
        /* Travese all non-zero document-topic distribution */
        for (const auto& k : n_mks[m])
        {
            psum += k.val * q[w].forest[0][k.idx].weight;
            p[ii++] = psum;
        }

        double u = rng_.rand_double() * (psum + alphaK*q[w].wsum);

        if (u < psum)
        {
            unsigned short temp = std::lower_bound(p, p+ii, u) - p;
            topic = n_mks[m].idx_in(temp);

            // add newly estimated z_i to count variables
            add_to_topic( w, m, topic );

            q[w].forest[0][old_topic].weight = copy_old_val;
            q[w].wsum += copy_old_val - temp_calc;
            if (topic != old_topic)
            {
                q[w].update(old_topic, temp_calc);
                temp_calc = (n_wk[w][topic] + beta) / (n_k[topic] + Vbeta);
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
            add_to_topic( w, m, topic );

            temp_calc = (n_wk[w][topic] + beta) / (n_k[topic] + Vbeta);
            q[w].update(topic, temp_calc);
        }

        z[m][n] = topic;
    }
    for (const auto& k : n_mks[m])
    {
            nd_m[k.idx] = 0;
            rev_mapper[k.idx] = -1;
    }
    return 0;
}

int lightLDA::specific_init()
{
    std::cout << "Initializing the alias tables ..." << std::endl;
    q.resize(V);
    for (unsigned w = 0; w < V; ++w)
    {
        q[w].init(K);
        generateQtable(w);
    }
    return 0;
}

int lightLDA::sampling(unsigned m)
{
    int kc = 0;
    for (const auto& k : n_mks[m])
    {
        nd_m[k.idx] = k.val;
        rev_mapper[k.idx] = kc++;
    }

    double sumPd = trngdata->docs[m]->length + alpha;

    for (unsigned n = 0; n < trngdata->docs[m]->length; ++n)
    {
        unsigned w = trngdata->docs[m]->words[n];

        // remove z_ij from the count variables
        unsigned short topic = z[m][n]; unsigned short new_topic; unsigned short old_topic = topic;
        remove_from_topic( w, m, topic);

        // MHV to draw new topic
        for (unsigned r = 0; r < MH_STEPS; ++r)
        {
            {
                // Draw a topic from doc-proposal
                double u = utils::unif01() * sumPd;
                if (u < trngdata->docs[m]->length)
                {
                    // draw from doc-topic distribution skipping n
                    unsigned pos = (unsigned)(u);
                    new_topic = z[m][pos];
                }
                else
                {
                    // draw uniformly
                    u -= trngdata->docs[m]->length;
                    u /= alphaK;
                    new_topic = (unsigned short)(u); // pick_a_number(0,trngdata->docs[m]->length-1); (int)(utils::unif01()*ptrndata->docs[m]->length);
                }

                if (topic != new_topic)
                {
                    //2. Find acceptance probability
                    double temp_old = (nd_m[topic] + alphaK) * (n_wk[w][topic] + beta) / (n_k[topic] + Vbeta) ;
                    double temp_new = (nd_m[new_topic] + alphaK) * (n_wk[w][new_topic] + beta) / (n_k[new_topic] + Vbeta);
                    double prop_old = (topic==old_topic) ? (nd_m[topic] + 1 + alphaK) : (nd_m[topic] + alphaK);
                    double prop_new = (new_topic==old_topic) ? (nd_m[new_topic] + 1 + alphaK) : (nd_m[new_topic] + alphaK);
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
                q[w].noSamples++;
                if(q[w].noSamples > K>>1 )
                {
                    generateQtable(w);
                }
                new_topic = q[w].sample(rng_.rand_k(K), rng_.rand_double());

			
                if (topic != new_topic)
                {
                    //2. Find acceptance probability
                    double temp_old = (nd_m[topic] + alphaK) * (n_wk[w][topic] + beta) / (n_k[topic] + Vbeta);
                    double temp_new = (nd_m[new_topic] + alphaK) * (n_wk[w][new_topic] + beta) / (n_k[new_topic] + Vbeta);
                    double acceptance =  (temp_new * q[w].w[topic]) / (temp_old * q[w].w[new_topic]);

                    //3. Compare against uniform[0,1]
                    if (rng_.rand_double() < acceptance)
                    {
                        topic = new_topic;
                    }
                }
            }
        }

        // add newly estimated z_i to count variables
        add_to_topic( w, m, topic );
        z[m][n] = topic;
    }
    for (const auto& k : n_mks[m])
    {
        nd_m[k.idx] = 0;
        rev_mapper[k.idx] = K;
    }
    return 0;
}

void lightLDA::generateQtable(unsigned w)
{
    q[w].wsum = 0.0;
    for (unsigned short k = 0; k < K; ++k)
    {
        q[w].w[k] = (n_wk[w][k] + beta) / (n_k[k] + Vbeta);
        q[w].wsum += q[w].w[k];
    }
    q[w].constructTable();
}
