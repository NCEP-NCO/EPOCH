// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// © University Corporation for Atmospheric Research (UCAR) 2009-2010. 
// All rights reserved.  The Government's right to use this data and/or 
// software (the "Work") is restricted, per the terms of Cooperative 
// Agreement (ATM (AGS)-0753581 10/1/08) between UCAR and the National 
// Science Foundation, to a "nonexclusive, nontransferable, irrevocable, 
// royalty-free license to exercise or have exercised for or on behalf of 
// the U.S. throughout the world all the exclusive rights provided by 
// copyrights.  Such license, however, does not include the right to sell 
// copies or phonorecords of the copyrighted works to the public."   The 
// Work is provided "AS IS" and without warranty of any kind.  UCAR 
// EXPRESSLY DISCLAIMS ALL OTHER WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
// ANY IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
// PURPOSE.  
//  
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
/**
 *  @file Histogram.cc
 *  @brief Source for Histogram class
 */

#include <ConvWx/Histogram.hh>

using std::vector;
using std::list;
 
/**
 * Missing value
 */
static const float threshMissing = -9999.0;

Histogram::Histogram(const vector<float> binBounds, 
		     const float valThresh):
  pBinBounds(binBounds),
  pValThresh(valThresh)
{ 
}

Histogram::Histogram(const vector<float> binBounds):
  pBinBounds(binBounds),
  pValThresh(threshMissing)
{ 
}

Histogram::~Histogram()
{
  pBinBounds.erase( pBinBounds.begin(), pBinBounds.end());
}


void Histogram::binData(const Grid &dataGrid,  vector < vector <int> > &hist, 
			int &numMissing, int &numOverThresh)const
{
  
  //
  // Clear the histogram and resize
  //
  for(int i = 0; i < static_cast<int>(hist.size()); i++)
  {
    hist[i].erase(hist[i].begin(), hist[i].end());
  }
  
  int numBins = static_cast<int>(pBinBounds.size()) - 1;

  hist.resize(numBins);
  
  vector < list < IndexGridVal* > > tmpHist;
  
  tmpHist.resize(numBins);
  
  int overThresh = 0;
  
  int missing  = 0;
  
  for ( int i = 0; i < dataGrid.getNdata(); i++)
  { 
    if (dataGrid.isMissingAt(i))
    {
      missing++;
    }
    else
    {
      double gridVal = dataGrid.returnValue(i);
      
      if (gridVal > pValThresh)
      {
	overThresh++;
      }
      
      for (int j = 0; j < numBins; j++)
      {
	if ( gridVal >= pBinBounds[j] && 
	     gridVal <  pBinBounds[j+1] )
	{
	  IndexGridVal *indexVilVal = new IndexGridVal(i, gridVal); 
	  
	  tmpHist[j].push_back(indexVilVal);
	  
	  j = numBins;
	}
      }
    }// end if
  }// end for

  numMissing = missing;
 
  numOverThresh = overThresh;
  
  //
  // Sort Histogram lists in ascending order, copy lists to vectors 
  // 
  list<IndexGridVal*>::iterator iter; 
  
  for (int i = 0; i < numBins; i++)
  {    
    IndexGridValPtrCmp indexGridValPtrCmp;
    
    (tmpHist[i]).sort(indexGridValPtrCmp);
    
    int numVals = static_cast<int>(tmpHist[i].size());
    
    hist[i].resize(numVals);
    
    iter = tmpHist[i].begin();
    
    for (int j = 0; j < numVals; j++)
    {
      hist[i][j] = static_cast<int>((*iter)->getIndex());
      
      iter++;  
    }
  }
  
  //
  // Cleanup
  //
  for (int i = 0; i < numBins; i++)
  {
    for (iter = tmpHist[i].begin(); iter != tmpHist[i].end(); iter++)
    {
      delete *iter;
    }
    
    tmpHist[i].erase( tmpHist[i].begin(), tmpHist[i].end());
  }
}

void Histogram::binData( const Grid &dataGrid,
                        vector <int> & histBinTot,
                        int &numMissing, int &numOverThresh)const
{
  //
  // Clear the histogram and resize
  //
  histBinTot.clear();
 
  int numBins = static_cast<int>(pBinBounds.size()) -1;
 
  histBinTot.resize(numBins);

  numOverThresh = 0;

  numMissing  = 0;

  for ( int i = 0; i < dataGrid.getNdata(); i++)
  {
   
    if (dataGrid.isMissingAt(i))
    {
      numMissing++;
    }
    else
    {
      double gridVal = dataGrid.returnValue(i);

      if (gridVal > pValThresh)
      {
        numOverThresh++;
      }
      for (int j = 0; j < numBins; j++)
      {
        if ( gridVal >= pBinBounds[j] &&
             gridVal <  pBinBounds[j+1] )
        {
          histBinTot[j]++;

          j = numBins;
        }
      }
    }// End if
  }// End for
}


void Histogram::binData(const Grid &indexGrid, const Grid &dataGrid,  
			vector < vector <int> > &hist, 
			int &numMissing, int &numOverThresh)const
{
  //
  // Clear the histogram and resize
  //
  for(int i = 0; i < static_cast<int>(hist.size()); i++)
  {
    hist[i].erase(hist[i].begin(), hist[i].end());
  }
  
  int numBins = static_cast<int>(pBinBounds.size()) - 1;
  
  hist.resize(numBins);
  
  vector < list < IndexGridVal* > > tmpHist;
  
  tmpHist.resize(numBins);
  
  int overThresh = 0;
  
  int missing  = 0;
  
  for ( int i = 0; i < indexGrid.getNdata(); i++)
  {
    double modelIndex;
    
    indexGrid.getValue(i, modelIndex);
    
    if (dataGrid.isMissingAt(static_cast<int>(modelIndex)))
    {
      missing++;
    }
    else
    {
      double gridVal = dataGrid.returnValue(static_cast<int>(modelIndex));
      
      if (gridVal > pValThresh)
      {
	overThresh++;
      }
      
      for (int j = 0; j < numBins; j++)
      {
	if ( gridVal >= pBinBounds[j] && 
	     gridVal <  pBinBounds[j+1] )
	{
	  IndexGridVal *indexVilVal = new IndexGridVal(modelIndex, gridVal); 
	  
	  tmpHist[j].push_back(indexVilVal);
	  
	  j = numBins;
	}
      }
    }// end if
  }// end for
  
  numMissing = missing;
  
  numOverThresh = overThresh;
  
  //
  // Sort Histogram lists in ascending order, copy lists to vectors 
  // 
  list<IndexGridVal*>::iterator iter; 
  
  for (int i = 0; i < numBins; i++)
  {    
    IndexGridValPtrCmp indexGridValPtrCmp;
    
    (tmpHist[i]).sort(indexGridValPtrCmp);
    
    int numVals = static_cast<int>(tmpHist[i].size());
    
    hist[i].resize(numVals);
    
    iter = tmpHist[i].begin();
    
    for (int j = 0; j < numVals; j++)
    {
      hist[i][j] = static_cast<int>((*iter)->getIndex());
      
      iter++;  
    }
  }
  
  //
  // Cleanup
  //
  for (int i = 0; i < numBins; i++)
  {
    for (iter = tmpHist[i].begin(); iter != tmpHist[i].end(); iter++)
    {
      delete *iter;
    }
    
    tmpHist[i].erase( tmpHist[i].begin(), tmpHist[i].end());
  }
}

void Histogram::binData(const Grid &indexGrid, const Grid &dataGrid,  
			vector <int> & histBinTot, 
			int &numMissing, int &numOverThresh)const
{
  //
  // Clear the histogram and resize
  //
  histBinTot.clear();
  
  int numBins = static_cast<int>(pBinBounds.size()) -1;
  
  histBinTot.resize(numBins);

  numOverThresh = 0;

  numMissing  = 0;
 
  for ( int i = 0; i < indexGrid.getNdata(); i++)
  {
    double modelIndex;
    
    indexGrid.getValue(i, modelIndex);
    
    if (dataGrid.isMissingAt(static_cast<int>(modelIndex)))
    {
      numMissing++;
    }
    else
    {
      double gridVal = dataGrid.returnValue(static_cast<int>(modelIndex));
      
      if (gridVal > pValThresh)
      {
	numOverThresh++;
      }
      for (int j = 0; j < numBins; j++)
      {
	if ( gridVal >= pBinBounds[j] && 
	     gridVal <  pBinBounds[j+1] )
	{
	  histBinTot[j]++;
	  
	  j = numBins;
	}
      }
    }// End if
  }// End for
}

void Histogram::binData(const Grid &indexGrid, const Grid &dataGrid,  
			vector <int> & histBinTot)const
{
  //
  // Clear the histogram and resize
  //
  histBinTot.clear();
  
  int numBins = static_cast<int>(pBinBounds.size()) -1;
  
  histBinTot.resize(numBins);

  for ( int i = 0; i < indexGrid.getNdata(); i++)
  {
    double modelIndex;
    
    indexGrid.getValue(i, modelIndex);
    
    if (dataGrid.isMissingAt(static_cast<int>(modelIndex)))
    {
      continue;
    }
    else
    {
      double gridVal = dataGrid.returnValue(static_cast<int>(modelIndex));
      
      for (int j = 0; j < numBins; j++)
      {
	if ( gridVal >= pBinBounds[j] && 
	     gridVal <  pBinBounds[j+1] )
	{
	  histBinTot[j]++;
	  
	  j = numBins;
	}
      }
    }// End if
  }// End for
}
