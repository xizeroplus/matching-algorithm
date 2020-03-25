#include "motree.h"


void Motree::insertSub(const IntervalSub &sub, int &matchPubs, int64_t &insertTime, int64_t &matchTime, const vector<Pub> &pubList)
{
	//match pub

	Timer matchStart;
	vector<vector<int> > submark(mLevel);
	//handle lng
	double low = (sub.lowlng-LNGMIN)/(LNGMAX-LNGMIN), high = (sub.highlng-LNGMIN)/(LNGMAX-LNGMIN);
	if(low<0 || high >1)
		submark[0].push_back(pubcells-1);
	int lowunit = (int)(low*(pubcells-1)), highunit = (int)(high*(pubcells-1));
	if(lowunit<0)lowunit=0;
	if(highunit>pubcells-2)highunit=pubcells-2;
	for(int i=lowunit; i<=highunit; ++i)
		submark[0].push_back(i);
	//handle lat
	low = (sub.lowlat-LATMIN)/(LATMAX-LATMIN), high = (sub.highlat-LATMIN)/(LATMAX-LATMIN);
	if(low<0 || high >1)
		submark[1].push_back(pubcells-1);
	lowunit = (int)(low*(pubcells-1)); highunit = (int)(high*(pubcells-1));
	if(lowunit<0)lowunit=0;
	if(highunit>pubcells-2)highunit=pubcells-2;
	for(int i=lowunit; i<=highunit; ++i)
		submark[1].push_back(i);
	//handle others
	for (int i = 0; i < sub.size; i++){
		int att = sub.constraints[i].att;
		if (att < mLevel-2){
			low = sub.constraints[i].lowValue / (double) valDom; high = sub.constraints[i].highValue / (double) valDom;
			lowunit = (int)(low*(pubcells-1)); highunit = (int)(high*(pubcells-1));
			if(highunit>pubcells-2)highunit=pubcells-2;
			for(int j=lowunit; j<=highunit; ++j)
				submark[att+2].push_back(j);
		}
	}
	
	for (int i = 2; i < mLevel; i++)
        if (submark[i].size() == 0)
            for (int j=0;j<pubcells;++j)
				submark[i].push_back(j);
			

	matchInPubdata(submark, 0, 0, matchPubs, pubList, sub);
	matchTime = matchStart.elapsed_nano();

	
	
	

	//insert sub
	Timer insertStart;
    vector<int> mark(mLevel, 0);
	//handle lng
	if(sub.highlng >= LNGMAX || sub.lowlng < LNGMIN || sub.highlng-sub.lowlng >= LNGCELL)
		mark[0] = 9;
	else{
		double center = ((sub.lowlng+sub.highlng)/2-LNGMIN)/(LNGMAX-LNGMIN);
		mark[0] = (int)((center-0.05)/0.1);
		if(mark[0]<0)mark[0]=0;
		else if(mark[0]>8)mark[0] = 8;
	}
	//handle lat
	if(sub.highlat >= LATMAX || sub.lowlat < LATMIN || sub.highlat-sub.lowlat >= LATCELL)
		mark[1] = 9;
	else{
		double center = ((sub.lowlat+sub.highlat)/2-LATMIN)/(LATMAX-LATMIN);
		mark[1] = (int)((center-0.05)/0.1);
		if(mark[1]<0)mark[1]=0;
		else if(mark[1]>8)mark[1] = 8;
	}
	//handle others
    for (int i = 0; i < sub.size; i++)
    {
        int att = sub.constraints[i].att;
        if (att < mLevel-2)
        {
			double low = sub.constraints[i].lowValue / (double) valDom, high = sub.constraints[i].highValue / (double) valDom;
			double width = high - low;
			int currentLevel = level-1-(int)(width / levelTarget);
			int cellInLevel = (int)(low/startDis[currentLevel]);
			if(cellInLevel > currentLevel) cellInLevel = currentLevel;
			int base = currentLevel*(currentLevel+1)/2;
			if(high < cellInLevel * startDis[currentLevel] + cellWidth[currentLevel]){ //cellInLevel can store, continue to check if cellInLevel-1 can store
				if(cellInLevel > 0 && high < (cellInLevel-1) * startDis[currentLevel] + cellWidth[currentLevel]){ //if true, check the distance of two centers
					double center = (high + low)/2;
					if(abs(center - (cellInLevel * startDis[currentLevel] + cellWidth[currentLevel]/2)) > abs(center - ((cellInLevel-1) * startDis[currentLevel] + cellWidth[currentLevel]/2))){
						mark[att+2] = base + cellInLevel-1;
					}
					else{
						mark[att+2] = base + cellInLevel;
					}
				}
				else{
					mark[att+2] = base + cellInLevel;
				}
			}
			else{ //if this level cannot store, then go to the higher level
				--currentLevel;
				cellInLevel = (int)(low/startDis[currentLevel]); //just repeat
				if(cellInLevel > currentLevel) cellInLevel = currentLevel;
				base = currentLevel*(currentLevel+1)/2;
				if(cellInLevel > 0 && high < (cellInLevel-1) * startDis[currentLevel] + cellWidth[currentLevel]){
					double center = (high + low)/2;
					if(abs(center - (cellInLevel * startDis[currentLevel] + cellWidth[currentLevel]/2)) > abs(center - ((cellInLevel-1) * startDis[currentLevel] + cellWidth[currentLevel]/2))){
						mark[att+2] = base + cellInLevel-1;
					}
					else{
						mark[att+2] = base + cellInLevel;
					}
				}
				else{
					mark[att+2] = base + cellInLevel;
				}
			}
		}
    }

	insertToBucket(mark, 0, 0, sub.id);
	insertTime = insertStart.elapsed_nano();



}


void Motree::insertToBucket(const vector<int> &mark, int bucketID, int layer, int subID)
{
    if (layer == mLevel - 1)
    {
        subdata[bucketID + mark[layer]].push_back(subID);
        return;
    }
	if(layer < 2)
		insertToBucket(mark, (bucketID + mark[layer]) * 10, layer + 1, subID);
	else
		insertToBucket(mark, (bucketID + mark[layer]) * subcells, layer + 1, subID);
}

void Motree::insertToPubdata(const vector<int> &mark, int bucketID, int layer, int pubID)
{
    if (layer == mLevel - 1)
    {
        pubdata[bucketID + mark[layer]].push_back(pubID);
        return;
    }
	insertToPubdata(mark, (bucketID + mark[layer]) * pubcells, layer + 1, pubID);
}









void Motree::insertPub(const Pub &pub, int &matchSubs, int64_t &insertTime, int64_t &matchTime, const vector<IntervalSub> &subList)
{
	
	//match sub
	Timer matchStart;
    vector<vector<int> > mark(mLevel);
	//handle lng
	double lng = (pub.lng-LNGMIN)/(LNGMAX-LNGMIN);
	mark[0].push_back(9);
	if(lng >= 0 && lng < 1){
		int unit = (int)(lng/0.1);
		if(unit>9) unit=9;
		if(unit<9) mark[0].push_back(unit);
		if(unit>0) mark[0].push_back(unit-1);
	}
	//handle lat
	double lat = (pub.lat-LATMIN)/(LATMAX-LATMIN);
	mark[1].push_back(9);
	if(lat >= 0 && lat < 1){
		int unit = (int)(lat/0.1);
		if(unit>9) unit=9;
		if(unit<9) mark[1].push_back(unit);
		if(unit>0) mark[1].push_back(unit-1);
	}
	//handle others
	for (int i = 0; i < pub.size; i++){
		int att = pub.pairs[i].att;
		if (att < mLevel-2){
			double value = pub.pairs[i].value / (double) valDom;
			mark[att+2].push_back(0);
			for(int currentLevel = 1; currentLevel < level; ++currentLevel){
				int cellInLevel = (int)(value/startDis[currentLevel]);
				if(cellInLevel > currentLevel) cellInLevel = currentLevel;
				int base = currentLevel*(currentLevel+1)/2;
				mark[att+2].push_back(base + cellInLevel);
				while(cellInLevel > 0 && value < (cellInLevel-1) * startDis[currentLevel] + cellWidth[currentLevel]){ //continue to check if cellInLevel-1 can match
					--cellInLevel;
					mark[att+2].push_back(base + cellInLevel);
				}
			}
		}
	}
	for (int i = 2; i < mLevel; i++)
        if (mark[i].size() == 0)
				mark[i].push_back(0);

	matchInBucket(mark, 0, 0, matchSubs, subList, pub);
	matchTime = matchStart.elapsed_nano();


	
	
	//insert pub
	Timer insertStart;
	vector<int> pubmark(mLevel, pubcells-1);
	//handle lng
	if(lng < 0 || lng >1)
		pubmark[0] = pubcells-1;
	else{
		int lngunit = (int)(lng*(pubcells-1));
		if(lngunit>pubcells-2)lngunit=pubcells-2;
		pubmark[0] = lngunit;
	}
	//handle lat
	if(lat < 0 || lat >1)
		pubmark[1] = pubcells-1;
	else{
		int latunit = (int)(lat*(pubcells-1));
		if(latunit>pubcells-2)latunit=pubcells-2;
		pubmark[1] = latunit;
	}
	//handle others
	for (int i = 0; i < pub.size; i++){
		int att = pub.pairs[i].att;
		if (att < mLevel-2){
			 double value = pub.pairs[i].value / (double) valDom;
			 int valueunit = (int)(value*(pubcells-1));
			 if(valueunit>pubcells-2)valueunit=pubcells-2;
			 pubmark[att+2] = valueunit;
		}
	}
	insertToPubdata(pubmark, 0, 0, pub.id);
	insertTime = insertStart.elapsed_nano();
	
	
    
}


void Motree::matchInBucket(const vector<vector<int> > &mark, int bucketID, int layer, int &matchSubs, const vector<IntervalSub> &subList, const Pub &pub)
{
    if (layer == mLevel - 1)
    {
        for (int i = 0; i < mark[layer].size(); i++)
        {
            int id = bucketID + mark[layer][i];
            for (int k = 0; k < subdata[id].size(); k++)
            {
                const IntervalSub &sub = subList[subdata[id][k]];
				if(!(pub.lng > sub.lowlng && pub.lng < sub.highlng && pub.lat > sub.lowlat && pub.lat < sub.highlat))
					continue;
                bool flag = true;
                for (int j = 0; j < sub.size; j++)
                {
                    int att = sub.constraints[j].att;
					int attinpub = -1;
					for(int jj=0; jj<pub.size; ++jj){
						if(pub.pairs[jj].att==att){
							attinpub = jj;
							break;
						}
					}
					if(attinpub == -1){
						flag = false;
                        break;
					}
                    if (pub.pairs[attinpub].value < sub.constraints[j].lowValue || pub.pairs[attinpub].value > sub.constraints[j].highValue)
                    {
                        flag = false;
                        break;
                    }
                }

                if (flag)
                {
                    ++matchSubs;
					//send sub.id
                }
            }
        }
        return;
    }
	if(layer < 2){
		for (int i = 0; i < mark[layer].size(); i++)
			matchInBucket(mark, (bucketID + mark[layer][i]) * 10, layer + 1, matchSubs, subList, pub);
	}else{
		for (int i = 0; i < mark[layer].size(); i++)
			matchInBucket(mark, (bucketID + mark[layer][i]) * subcells, layer + 1, matchSubs, subList, pub);
	}
}

void Motree::matchInPubdata(const vector<vector<int> > &mark, int bucketID, int layer, int &matchPubs, const vector<Pub> &pubList, const IntervalSub &sub)
{
    if (layer == mLevel - 1)
    {
        for (int i = 0; i < mark[layer].size(); i++)
        {
            int id = bucketID + mark[layer][i];
            for (int k = 0; k < pubdata[id].size(); k++)
            {
                const Pub &pub = pubList[pubdata[id][k]];
				/*
				here should check whether pub is out of time
				if it is, simply delete pubdata[id][k], just like what "deleteFromPubdata" function does (line 495)
				because other algorithms we compared with have no information about time
				for fair comparison of "matching time", the common data structure does not add "time"
				*/
				if(!(pub.lng > sub.lowlng && pub.lng < sub.highlng && pub.lat > sub.lowlat && pub.lat < sub.highlat))
					continue;
				bool flag = true;
                for (int j = 0; j < sub.size; j++)
                {
					int att = sub.constraints[j].att;
					int attinpub = -1;
					for(int jj=0; jj<pub.size; ++jj){
						if(pub.pairs[jj].att==att){
							attinpub = jj;
							break;
						}
					}
					if(attinpub == -1){
						flag = false;
                        break;
					}
                    if (pub.pairs[attinpub].value < sub.constraints[j].lowValue || pub.pairs[attinpub].value > sub.constraints[j].highValue)
                    {
                        flag = false;
                        break;
                    }
                }

                if (flag)
                {
                    ++matchPubs;
					//send pub.id
                }
            }
        }
        return;
    }
	for (int i = 0; i < mark[layer].size(); i++)
		matchInPubdata(mark, (bucketID + mark[layer][i]) * pubcells, layer + 1, matchPubs, pubList, sub);
}

void Motree::deleteSub(const IntervalSub &sub, int64_t &deleteTime)
{

	
	//delete sub
	Timer deleteStart;
    vector<int> mark(mLevel, 0);
	double low, high;
	//handle lng
	if(sub.highlng >= LNGMAX || sub.lowlng < LNGMIN || sub.highlng-sub.lowlng >= LNGCELL)
		mark[0] = 9;
	else{
		double center = ((sub.lowlng+sub.highlng)/2-LNGMIN)/(LNGMAX-LNGMIN);
		mark[0] = (int)((center-0.05)/0.1);
		if(mark[0]<0)mark[0]=0;
		else if(mark[0]>8)mark[0] = 8;
	}
	//handle lat
	if(sub.highlat >= LATMAX || sub.lowlat < LATMIN || sub.highlat-sub.lowlat >= LATCELL)
		mark[1] = 9;
	else{
		double center = ((sub.lowlat+sub.highlat)/2-LATMIN)/(LATMAX-LATMIN);
		mark[1] = (int)((center-0.05)/0.1);
		if(mark[1]<0)mark[1]=0;
		else if(mark[1]>8)mark[1] = 8;
	}
	//handle others
    for (int i = 0; i < sub.size; i++)
    {
        int att = sub.constraints[i].att;
        if (att < mLevel-2)
        {
			low = sub.constraints[i].lowValue / (double) valDom; high = sub.constraints[i].highValue / (double) valDom;
			double width = high - low;
			int currentLevel = level-1-(int)(width / levelTarget);
			int cellInLevel = (int)(low/startDis[currentLevel]);
			if(cellInLevel > currentLevel) cellInLevel = currentLevel;
			int base = currentLevel*(currentLevel+1)/2;
			if(high < cellInLevel * startDis[currentLevel] + cellWidth[currentLevel]){ //cellInLevel can store, continue to check if cellInLevel-1 can store
				if(cellInLevel > 0 && high < (cellInLevel-1) * startDis[currentLevel] + cellWidth[currentLevel]){ //if true, check the distance of two centers
					double center = (high + low)/2;
					if(abs(center - (cellInLevel * startDis[currentLevel] + cellWidth[currentLevel]/2)) > abs(center - ((cellInLevel-1) * startDis[currentLevel] + cellWidth[currentLevel]/2))){
						mark[att+2] = base + cellInLevel-1;
					}
					else{
						mark[att+2] = base + cellInLevel;
					}
				}
				else{
					mark[att+2] = base + cellInLevel;
				}
			}
			else{ //if this level cannot store, then go to the higher level
				--currentLevel;
				cellInLevel = (int)(low/startDis[currentLevel]); //just repeat
				if(cellInLevel > currentLevel) cellInLevel = currentLevel;
				base = currentLevel*(currentLevel+1)/2;
				if(cellInLevel > 0 && high < (cellInLevel-1) * startDis[currentLevel] + cellWidth[currentLevel]){
					double center = (high + low)/2;
					if(abs(center - (cellInLevel * startDis[currentLevel] + cellWidth[currentLevel]/2)) > abs(center - ((cellInLevel-1) * startDis[currentLevel] + cellWidth[currentLevel]/2))){
						mark[att+2] = base + cellInLevel-1;
					}
					else{
						mark[att+2] = base + cellInLevel;
					}
				}
				else{
					mark[att+2] = base + cellInLevel;
				}
			}
		}
    }
	deleteFromBucket(mark, 0, 0, sub.id);
	deleteTime = deleteStart.elapsed_nano();
}

void Motree::deleteFromBucket(const vector<int> &mark, int bucketID, int layer, int subID)
{
    if (layer == mLevel - 1)
    {
		int currentbucket = bucketID + mark[layer];
		for(int i=0;i<subdata[currentbucket].size();++i){
			if(subdata[currentbucket][i] == subID){
				subdata[currentbucket].erase(subdata[currentbucket].begin()+i);
				return;
			}
		}
		cerr<<"cannot found sub "<<subID<<"in "<<currentbucket<<endl;
        return;
    }
	if(layer < 2)
		insertToBucket(mark, (bucketID + mark[layer]) * 10, layer + 1, subID);
	else
		insertToBucket(mark, (bucketID + mark[layer]) * subcells, layer + 1, subID);
}


void Motree::deletePub(const Pub &pub, int64_t &deleteTime)
{
	
	//delete pub
	Timer deleteStart;
	vector<int> pubmark(mLevel, pubcells-1);
	double lng = (pub.lng-LNGMIN)/(LNGMAX-LNGMIN), lat = (pub.lat-LATMIN)/(LATMAX-LATMIN);
	//handle lng
	if(lng < 0 || lng >1)
		pubmark[0] = pubcells-1;
	else{
		int lngunit = (int)(lng*(pubcells-1));
		if(lngunit>pubcells-2)lngunit=pubcells-2;
		pubmark[0] = lngunit;
	}
	//handle lat
	if(lat < 0 || lat >1)
		pubmark[1] = pubcells-1;
	else{
		int latunit = (int)(lat*(pubcells-1));
		if(latunit>pubcells-2)latunit=pubcells-2;
		pubmark[1] = latunit;
	}
	//handle others
	for (int i = 0; i < pub.size; i++){
		int att = pub.pairs[i].att;
		if (att < mLevel-2){
			 double value = pub.pairs[i].value / (double) valDom;
			 int valueunit = (int)(value*(pubcells-1));
			 if(valueunit>pubcells-2)valueunit=pubcells-2;
			 pubmark[att+2] = valueunit;
		}
	}
	deleteFromPubdata(pubmark, 0, 0, pub.id);
	deleteTime = deleteStart.elapsed_nano();
}

void Motree::deleteFromPubdata(const vector<int> &mark, int bucketID, int layer, int pubID)
{
	if (layer == mLevel - 1)
    {
		int currentbucket = bucketID + mark[layer];
		for(int i=0;i<pubdata[currentbucket].size();++i){
			if(pubdata[currentbucket][i] == pubID){
				pubdata[currentbucket].erase(pubdata[currentbucket].begin()+i);
				return;
			}
		}
		cerr<<"cannot found pub "<<pubID<<"in "<<currentbucket<<endl;
        return;
    }
	insertToPubdata(mark, (bucketID + mark[layer]) * pubcells, layer + 1, pubID);
}
