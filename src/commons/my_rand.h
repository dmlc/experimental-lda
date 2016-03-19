#ifndef MY_RAND_H_
#define MY_RAND_H_

#include <chrono>

/* This RNG passes TestU01 */
class xorshift128plus
{
public:
    xorshift128plus()
    {
        s[0] = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        s[1] = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    }
    ~xorshift128plus() {   }
    
    inline uint64_t rand()
    {
        uint64_t x = s[0];
        uint64_t const y = s[1];
        s[0] = y;
        x ^= x << 23; // a
        s[1] = x ^ y ^ (x >> 17) ^ (y >> 26); // b, c
        return (s[1] + y);
    }

    inline double rand_double()
    {
        //return ((uint32_t)(rand()&0xffffffff)) * 2.3283064e-10 ; // 5.4210108624275e-20;
        return rand() * 5.4210108624275e-20;
    }
    
    inline uint32_t rand_k(unsigned K)
    {
        //return static_cast<unsigned>(rand() * 4.6566125e-10 * K);
        return rand()%K;
    }
    
    inline uint32_t rand_b(unsigned B)
    {
        return rand() & ((1U << B)-1);
    }
    
    double rand_norm()
    {
        if (haveNextNextGaussian) {
            haveNextNextGaussian = false;
            return nextNextGaussian;
        } else {
            double v1, v2, s;
            do {
                v1 = 2 * rand_double() - 1; // between -1 and 1
                v2 = 2 * rand_double() - 1; // between -1 and 1
                s = v1 * v1 + v2 * v2;
            } while (s >= 1 || s == 0);
            double multiplier = sqrt(-2 * log(s) / s);
            nextNextGaussian = v2 * multiplier;
            haveNextNextGaussian = true;
            return v1 * multiplier;
        }
    }
    
    double rand_norm(double mu, double sigma)
    {
        if (haveNextNextGaussian) {
            haveNextNextGaussian = false;
            return nextNextGaussian * sigma + mu;
        } else {
            double v1, v2, s;
            do {
                v1 = 2 * rand_double() - 1; // between -1 and 1
                v2 = 2 * rand_double() - 1; // between -1 and 1
                s = v1 * v1 + v2 * v2;
            } while (s >= 1 || s == 0);
            double multiplier = sqrt(-2 * log(s) / s);
            nextNextGaussian = v2 * multiplier;
            haveNextNextGaussian = true;
            return v1 * multiplier * sigma + mu;
        }
    }
    
    double rand_gamma(double rr)
    {
        //return new GammaDistribution(rr, 1.0).sample();
	double bb, cc, dd;
	double uu, vv, xx, yy;
        double result;

	if (rr <= 0.0)                  // Not well defined, set to zero and skip.
            result = 0.0;
	else if (rr == 1.0)             // Exponential
            result = -log(rand_double());
	else if (rr < 1.0)
        {
            /* Johnks generator */
            cc = 1.0 / rr;
            dd = 1.0 / (1.0 - rr);
            do
            {
                xx = pow(rand_double(), cc);
		yy = xx + pow(rand_double(), dd);
            }
            while (yy > 1.0);
        
            result = -log(rand_double()) * xx / yy;
        }
        else                            // rr > 1.0
        { 
            /* Bests method */
//            bb = rr - 1.0;
//            cc = 3.0 * rr - 0.75;
//            while(true)
//            {
//                uu = rand_double();
//		vv = rand_double();
//		ww = uu * (1.0 - uu);
//		yy = sqrt(cc / ww) * (uu - 0.5);
//		xx = bb + yy;
//		if (xx >= 0)
//                {
//                    zz = 64.0 * ww * ww * ww * vv * vv;
//                    if ((zz <= (1.0 - 2.0 * yy * yy / xx)) || (log(zz) <= 2.0 * (bb * log(xx / bb) - yy)))
//                        break;
//                }
//            }
//            result = xx;
            
            /* Marsaglia and Tsang’s Method */
            bb=rr-1./3.;
            cc=1./sqrt(9.*bb);
            while(true)
            {
                xx=rand_norm();
                vv=1.+cc*xx;
                if(vv>=0)
                {
                    vv=vv*vv*vv; 
                    uu=rand_double();
                    if( (uu<=1.-.0331*(xx*xx)*(xx*xx)) || (log(uu)<=0.5*xx*xx+bb*(1.-vv+log(vv))) )
                        break;
                }
            }
            result = (bb*vv);
	}
        return result;
    }
    
    double rand_beta(double a, double b)
    {
        double x = rand_gamma(a);
        return x/(x+rand_gamma(b));
    }
    
    private:
    // No copying allowed
    xorshift128plus(const xorshift128plus &other) =delete;
    void operator=(const xorshift128plus &other) =delete;
    
    /* seed */
    uint64_t s[2];
    bool haveNextNextGaussian = false;
    double nextNextGaussian;
    
};

#endif
