#include "forest.h"

carry temp1;
my_stack<carry> Q1;

void forestSample::processQueue()
{
	double newW;
	char t;
	short l, pos, m;
	short curP, newP;

	while (Q1.size())
	{
		temp1 = Q1.top(); Q1.pop();

		t = temp1.task;
		l = temp1.level;
		pos = temp1.pos;
		m = temp1.addr;
		newW = temp1.dw;

		curP = utils::discreteLog(forest[l][pos].weight);
		newP = utils::discreteLog(forest[l][pos].weight + newW);

		if (t == 1)		//Inserstion of a child
		{
			if (forest[l][pos].children.size() > 1)
			{
				if (curP != newP)
				{
					//Add old parent to processing queue for deletion
					temp1.task = -1;
					temp1.level = l + 1;
					temp1.pos = curP;
					temp1.addr = forest[l][pos].childNum;
					temp1.dw = -forest[l][pos].weight;
					Q1.push(temp1);

					//Insert new child and Update weight
					forest[l - 1][m].childNum = forest[l][pos].children.size();
					forest[l][pos].children.push_back(m);
					forest[l][pos].weight += newW;

					//Add new parent to processing queue for insertion
					temp1.task = 1;
					temp1.level = l + 1;
					temp1.pos = newP;
					temp1.addr = pos;
					temp1.dw = forest[l][pos].weight;
					Q1.push(temp1);
				}

				else
				{
					//Insert new child and Update weight
					forest[l - 1][m].childNum = forest[l][pos].children.size();
					forest[l][pos].children.push_back(m);
					forest[l][pos].weight += newW;

					//Add parent to processing queue for updation
					temp1.task = 0;
					temp1.level = l + 1;
					temp1.pos = newP;
					temp1.addr = -1;
					temp1.dw = newW;
					Q1.push(temp1);
				}
			}
			else if (forest[l][pos].children.size() == 1)
			{
				//remove from table
				forest[l][levels[l].children.back()].childNum = forest[l][pos].childNum;
				levels[l].children[forest[l][pos].childNum] = levels[l].children.back();
				levels[l].children.pop_back();
				levels[l].weight -= forest[l][pos].weight;

				//Insert new child and Update weight
				forest[l - 1][m].childNum = 1;
				forest[l][pos].children.push_back(m);
				forest[l][pos].weight += newW;

				//Add new parent to processing queue for insertion
				temp1.task = 1;
				temp1.level = l + 1;
				temp1.pos = newP;
				temp1.addr = pos;
				temp1.dw = forest[l][pos].weight;
				Q1.push(temp1);
			}
			else
			{
				//Insert new child and Update weight
				forest[l - 1][m].childNum = 0;
				forest[l][pos].children.push_back(m);
				forest[l][pos].weight += newW;

				//add to table
				forest[l][pos].childNum = levels[l].children.size();
				levels[l].weight += forest[l][pos].weight;
				levels[l].children.push_back(pos);
			}
		}
		else if (t == 0) //updation
		{
			if (forest[l][pos].children.size() > 1)
			{
				if (curP != newP)
				{
					//Add old parent to processing queue for deletion
					temp1.task = -1;
					temp1.level = l + 1;
					temp1.pos = curP;
					temp1.addr = forest[l][pos].childNum;
					temp1.dw = -forest[l][pos].weight;
					Q1.push(temp1);

					//Update weight
					forest[l][pos].weight += newW;

					//Add new parent to processing queue for insertion
					temp1.task = 1;
					temp1.level = l + 1;
					temp1.pos = newP;
					temp1.addr = pos;
					temp1.dw = forest[l][pos].weight;
					Q1.push(temp1);
				}
				else
				{
					//Update weight
					forest[l][pos].weight += newW;

					//Add parent to processing queue for updation
					temp1.task = 0;
					temp1.level = l + 1;
					temp1.pos = newP;
					temp1.addr = -1;
					temp1.dw = newW;
					Q1.push(temp1);
				}
			}
			else if (forest[l][pos].children.size() == 1)
			{
				//Update weight
				forest[l][pos].weight += newW;

				//Update in table
				levels[l].weight += newW;
			}
		}
		else if (t == -1)	//deletion
		{
			if (forest[l][pos].children.size() > 2)
			{
				if (curP != newP)
				{
					//Add old parent to processing queue for deletion
					temp1.task = -1;
					temp1.level = l + 1;
					temp1.pos = curP;
					temp1.addr = forest[l][pos].childNum;
					temp1.dw = -forest[l][pos].weight;
					Q1.push(temp1);

					//Remove old child and Update weight
					//forest[l - 1][forest[l][pos].children.back()].childNum = m*(m != forest[l][pos].children.size() - 1) + forest[l][pos].children.back()*(m == forest[l][pos].children.size() - 1);
					//forest[l][pos].children[m] = forest[l][pos].children.back();
					if (m != forest[l][pos].children.size() - 1)
					{
						forest[l - 1][forest[l][pos].children.back()].childNum = m;
						forest[l][pos].children[m] = forest[l][pos].children.back();
					}
					forest[l][pos].children.pop_back();
					forest[l][pos].weight += newW;

					//Add new parent to processing queue for insertion
					temp1.task = 1;
					temp1.level = l + 1;
					temp1.pos = newP;
					temp1.addr = pos;
					temp1.dw = forest[l][pos].weight;
					Q1.push(temp1);
				}

				else
				{
					//Remove old child and Update weight
					//forest[l - 1][forest[l][pos].children.back()].childNum = m*(m != forest[l][pos].children.size() - 1) + forest[l][pos].children.back()*(m == forest[l][pos].children.size() - 1);
					//forest[l][pos].children[m] = forest[l][pos].children.back();
					if (m != forest[l][pos].children.size() - 1)
					{
						forest[l - 1][forest[l][pos].children.back()].childNum = m;
						forest[l][pos].children[m] = forest[l][pos].children.back();
					}
					forest[l][pos].children.pop_back();
					forest[l][pos].weight += newW;

					//Add parent to processing queue for updation
					temp1.task = 0;
					temp1.level = l + 1;
					temp1.pos = newP;
					temp1.addr = -1;
					temp1.dw = newW;
					Q1.push(temp1);
				}
			}
			else if (forest[l][pos].children.size() == 2)
			{
				//Add old parent to processing queue for deletion
				temp1.task = -1;
				temp1.level = l + 1;
				temp1.pos = curP;
				temp1.addr = forest[l][pos].childNum;
				temp1.dw = -forest[l][pos].weight;
				Q1.push(temp1);

				//Remove old child and Update weight
				//forest[l - 1][forest[l][pos].children.back()].childNum = m*(m != forest[l][pos].children.size() - 1) + forest[l][pos].children.back()*(m == forest[l][pos].children.size() - 1);
				//forest[l][pos].children[m] = forest[l][pos].children.back();
				if (m != forest[l][pos].children.size() - 1)
				{
					forest[l - 1][forest[l][pos].children.back()].childNum = m;
					forest[l][pos].children[m] = forest[l][pos].children.back();
				}
				forest[l][pos].children.pop_back();
				forest[l][pos].weight += newW;

				//add to table
				forest[l][pos].childNum = levels[l].children.size();
				levels[l].weight += forest[l][pos].weight;
				levels[l].children.push_back(pos);
			}
			else
			{
				//remove from table
				forest[l][levels[l].children.back()].childNum = forest[l][pos].childNum;
				levels[l].children[forest[l][pos].childNum] = levels[l].children.back();
				levels[l].children.pop_back();
				levels[l].weight -= forest[l][pos].weight;

				//Remove old child and reset weight
				forest[l][pos].weight = 0;
				forest[l][pos].childNum = -1;
				forest[l][pos].children.clear();
			}
		}
	}
}

forestSample::forestSample()
{
	N = 0;
	//weights = NULL;
	wsum = 0;
	levels = new node[6];
	//forest = new node*[6];
	forest.resize(6);
	forest.shrink_to_fit();
	for (short i = 1; i < 6; ++i)
	{
		//forest[i] = new node[64];
		forest[i].resize(128);
		forest[i].shrink_to_fit();
	}
}

bool forestSample::preprocess(short n, double* w)	// Preprocess
{
	N = n;
	wsum = 0;
	//forest[0] = new node[N];
	forest[0].resize(N);
	forest[0].shrink_to_fit();
	for (short i = 0; i < N; ++i)
	{
		wsum += w[i];
		short j = utils::discreteLog(w[i]);
		forest[0][i].weight = w[i];
		forest[0][i].childNum = forest[1][j].children.size();
		forest[1][j].weight += w[i];
		forest[1][j].children.push_back(i);
	}

	for (short l = 1; l < 6; ++l)
	{
		/*for (auto i = forest[l].begin(); i != forest[l].end(); ++i)
		{
			if (i->second.children.size() > 1)
			{
				short j = utils::discreteLog(i->second.weight);
				i->second.childNum = forest[l + 1][j].children.size();
				forest[l + 1][j].weight += i->second.weight;
				forest[l + 1][j].children.push_back(i->first);
			}
			else if (i->second.children.size() == 1)
			{
				i->second.childNum = levels[l].children.size();
				levels[l].weight += i->second.weight;
				levels[l].children.push_back(i->first);
			}
		}*/
		for (short i = 0; i < 128; ++i)
		{
			if (forest[l][i].children.size()>1)
			{
				short j = utils::discreteLog(forest[l][i].weight);
				forest[l][i].childNum = forest[l + 1][j].children.size();
				forest[l + 1][j].weight += forest[l][i].weight;
				forest[l + 1][j].children.push_back(i);
			}
			else if (forest[l][i].children.size() == 1)
			{
				forest[l][i].childNum = levels[l].children.size();
				levels[l].weight += forest[l][i].weight;
				levels[l].children.push_back(i);
			}
		}
	}
	return true;
}

short forestSample::generate()
{
	// Step 1: Select level
	short l;
	double u = wsum * utils::unif01();
	for (l = 1; u > levels[l].weight; u -= levels[l++].weight);

	// Step 2: Select root node in the level
	short r;
	//u = levels[l].weight * utils::unif01();
	//for (r = levels[l].children.size(); u > 0; u -= forest[l][levels[l].children[--r]].weight);
	for (r = -1; u > 0; u -= forest[l][levels[l].children[++r]].weight);
	r = forest[l][levels[l].children[r]].children[0];

	// Step 3: Traverse down the tree
	for (--l; l > 0; --l)
	{
		short I;
		double weightI;
		do {
			u = forest[l][r].children.size() * utils::unif01();
			I = (int)(u); // floor(u);
			//Setting exponent to -1
			int64_t L = *(int64_t*)& forest[l - 1][forest[l][r].children[I]].weight;
			L = L & 0x800FFFFFFFFFFFFF;
			L = L | 0x3FE0000000000000;
			weightI = *(double*)&L;
		} while (u - I >= weightI);
		r = forest[l][r].children[I];
	}
	return r;
}

void forestSample::update(short pos, double newW)
{
	short curP, newP;
	wsum += newW - forest[0][pos].weight;

	curP = utils::discreteLog(forest[0][pos].weight);
	newP = utils::discreteLog(newW);

	if (curP != newP)
	{
		//Add old parent to processing queue for deletion
		temp1.task = -1;
		temp1.level = 1;
		temp1.pos = curP;
		temp1.addr = forest[0][pos].childNum;
		temp1.dw = -forest[0][pos].weight;
		Q1.push(temp1);

		//update weight
		forest[0][pos].weight = newW;

		//Add new parent to processing queue for insertion
		temp1.task = 1;
		temp1.level = 1;
		temp1.pos = newP;
		temp1.addr = pos;
		temp1.dw = forest[0][pos].weight;
		Q1.push(temp1);
	}
	else
	{
		//Add parent to processing queue for updation
		temp1.task = 0;
		temp1.level = 1;
		temp1.pos = newP;
		temp1.addr = -1;
		temp1.dw = newW - forest[0][pos].weight;
		Q1.push(temp1);

		//Update weight
		forest[0][pos].weight = newW;

	}

	processQueue();
}

void forestSample::add(double newW)
{
	short newP;
	node newOne;
	wsum += newW;

	newP = utils::discreteLog(newW);

	//update weight
	newOne.weight = newW;
	newOne.childNum = forest[1][newP].children.size();
	forest[0][N] = newOne;
	//forest[0].insert(std::pair<short,node>(N,newOne));
	forest[0].push_back(newOne);

	//Add new parent to processing queue for insertion
	temp1.task = 1;
	temp1.level = 1;
	temp1.pos = newP;
	temp1.addr = N;
	temp1.dw = forest[0][N].weight;
	Q1.push(temp1);

	++N;

	processQueue();
}

void forestSample::remove(short pos)
{
	short curP;
	node newOne;
	wsum -= forest[0][pos].weight;

	curP = utils::discreteLog(forest[0][pos].weight);

	//Add old parent to processing queue for deletion
	temp1.task = -1;
	temp1.level = 1;
	temp1.pos = curP;
	temp1.addr = forest[0][pos].childNum;
	temp1.dw = -forest[0][pos].weight;
	Q1.push(temp1);

	//update weight
	--N;
	curP = utils::discreteLog(forest[0][N].weight);
	forest[1][curP].children[forest[0][N].childNum] = pos;
	forest[0][pos] = forest[0][N];
	//forest[0].erase(N);
	forest[0].pop_back();

	processQueue();
}
