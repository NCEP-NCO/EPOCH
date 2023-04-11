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
 *  @file QuantileBins.cc
 *  @brief Source for QuantileBins class
 */

#include <ConvWx/QuantileBins.hh>
#include <cmath>

using std::vector;
using std::list;
 
/**
 * Missing value
 */
static const float threshMissing = -9999.0;

QuantileBins::QuantileBins(const int qQuantiles, 
			   const float valThresh):
  pQQuantiles(qQuantiles),
  pValThresh(valThresh)
{ 
}

QuantileBins::QuantileBins(const int qQuantiles):
  pQQuantiles(qQuantiles),
  pValThresh(threshMissing)
{ 

}

QuantileBins::~QuantileBins()
{
 
}

void QuantileBins::binData(const Grid &indexGrid, const Grid &dataGrid,  
			   vector < vector <int> > &hist, 
			   std::vector < float> &binAvg,
			   int &numMissing, int &numOverThresh, float lowerBound)const
{
  //
  // Clear the histogram and resize
  //
  for(int i = 0; i < static_cast<int>(hist.size()); i++)
  {
    hist[i].erase(hist[i].begin(), hist[i].end());
  }
  
  int numBins = static_cast<int>(pQQuantiles);
  
  hist.resize(numBins);

  binAvg.resize(numBins);
  
  list < IndexGridVal* > dataList;  

  //
  // Construct IndexGridVal objects and put data in an STL list
  // for sorting. Record missing and data above given threshold.
  // 
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
      
      if (gridVal > pValThresh && gridVal > lowerBound && !std::isnan(gridVal))
      {
	numOverThresh++;
      }  
     
      if( gridVal > lowerBound)
      {
         IndexGridVal *indexGridVal = new IndexGridVal(modelIndex, gridVal); 
	  
         dataList.push_back(indexGridVal);
      }
      else 
      {
         //numMissing++;
      }

    }// end if
  }// end for
  
  //
  // Sort data into QuantileBins in ascending order, copy lists to vectors 
  // 
  list<IndexGridVal*>::iterator iter; 
 
  IndexGridValPtrCmp indexGridValPtrCmp;
    
  dataList.sort(indexGridValPtrCmp);
    
  // int numValsPerBin = static_cast<int>(indexGrid.getNdata()/pQQuantiles);

  // int numRemainderVals =  indexGrid.getNdata() - (numValsPerBin * pQQuantiles);

  // int numValsFirstBin =  numValsPerBin + numRemainderVals;
  
  int numValsPerBin =  static_cast<int>(dataList.size()/pQQuantiles);

  int numRemainderVals =  static_cast<int>(dataList.size()) - (numValsPerBin * pQQuantiles);

  int numValsFirstBin =  numValsPerBin + numRemainderVals;

  //
  // Fill the first q-quantile. This bin may have more values than the others
  // 
  hist[0].resize(numValsFirstBin);
  
  float binTotal = 0;

  iter = dataList.begin(); 
  
  
  for ( int j = 0; j < numValsFirstBin; j++)
  {
    hist[0][j] = static_cast<int>( (*iter)->getIndex());

    binTotal = binTotal + (*iter)->getValue();

    iter++;
  }

  binAvg[0] = binTotal/numValsFirstBin;
  
  
  //
  // Fill the rest of the q-quantile bins evenly
  //
  for ( int i = 1; i < pQQuantiles; i++)
  {
    hist[i].resize(numValsPerBin);
   
    binTotal = 0;
 
    for (int j = 0; j <  numValsPerBin; j++)
    {
      hist[i][j] = static_cast<int>( (*iter)->getIndex());
      
      binTotal = binTotal + (*iter)->getValue();
      
      iter++;
    }
    binAvg[i] = binTotal/ numValsPerBin;
  }

  //
  // Cleanup
  //
  for (iter = dataList.begin(); iter != dataList.end(); iter++)
  {
    delete *iter;
  }
  
}


void QuantileBins::binData( const Grid &dataGrid,
			    vector < vector <int> > &hist,
			    std::vector < float> &binAvg, 
			    int &numMissing, int &numOverThresh, float lowerBound)const
{
  //
  // Clear the histogram and resize
  //
  for(int i = 0; i < static_cast<int>(hist.size()); i++)
  {
    hist[i].erase(hist[i].begin(), hist[i].end());
  }

  int numBins = static_cast<int>(pQQuantiles);

  hist.resize(numBins);

  binAvg.resize(numBins);

  list < IndexGridVal* > dataList;

  //
  // Construct IndexGridVal objects and put data in an STL list
  // for sorting. Record missing and data above given threshold.
  //
  for ( int i = 0; i < dataGrid.getNdata(); i++)
  {

    if (dataGrid.isMissingAt(i))
    {
      numMissing++;
    }
    else
    {
      double gridVal = dataGrid.returnValue(i);

      if (gridVal > pValThresh && gridVal > lowerBound)
      {
        numOverThresh++;
      }

      if (gridVal > lowerBound)
      {
        IndexGridVal *indexGridVal = new IndexGridVal(i, gridVal);

        dataList.push_back(indexGridVal);
     }
     else
     {
        //numMissing++;
     }

    }// end if
  }// end for

  //
  // Sort data into QuantileBins in ascending order, copy lists to vectors
  //
  list<IndexGridVal*>::iterator iter;

  IndexGridValPtrCmp indexGridValPtrCmp;

  dataList.sort(indexGridValPtrCmp);
  
  int numValsPerBin =  static_cast<int>(dataList.size()/pQQuantiles);

  int numRemainderVals =  static_cast<int>(dataList.size()) - (numValsPerBin * pQQuantiles);

  int numValsFirstBin =  numValsPerBin + numRemainderVals;
  //
  // Fill the first q-quantile. This bin may have more values than the others
  //
  hist[0].resize(numValsFirstBin);

  iter = dataList.begin();

  float binTotal = 0;

  for ( int j = 0; j < numValsFirstBin; j++)
  {
    hist[0][j] = static_cast<int>( (*iter)->getIndex());

    binTotal = binTotal +  (*iter)->getValue();

    iter++;
  }

  binAvg[0]= binTotal/numValsFirstBin;
  
  
  for ( int i = 1; i < pQQuantiles; i++)
  {
    hist[i].resize(numValsPerBin);

    binTotal= 0;

    for (int j = 0; j <  numValsPerBin; j++)
    {
      hist[i][j] = static_cast<int>( (*iter)->getIndex());

       binTotal = binTotal +  (*iter)->getValue();

      iter++;
    }
    binAvg[i] =  binTotal/numValsPerBin;
  }

  //
  // Cleanup
  //
  for (iter = dataList.begin(); iter != dataList.end(); iter++)
    {
      delete *iter;
    }
}

