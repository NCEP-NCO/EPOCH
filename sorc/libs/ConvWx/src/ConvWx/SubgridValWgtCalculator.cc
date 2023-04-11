/** 
 * @file SubgridValWgtCalculator.cc
 * @brief Source for SubgridValWgtCalculator class
 */
#include <cmath>
#include <ConvWx/SubgridValWgtCalculator.hh>
#include <ConvWx/ConvWxConstants.hh>

using std::vector;

//
// Value to indicate no area data available
//
const double SubgridValWgtCalculator::minArea = 1;

//
//  Minimum area or weight assigned to a grid point
//
const double SubgridValWgtCalculator::noArea = -1;

//
// Each point in rectangular grid with dimensions nx and ny
// is assigned an integer weight. The weights are determined by
// the rectangular area formed by the grid edges and the the 
// shortest distances in the vertical i( or N-S ) and 
// horizontal (or E-W) directions from the point to the closest 
// grid edges. Hence greater weight is given to interior points
// and less weight to the edges. The minimum area assigned to 
// any point is 1.  
//
SubgridValWgtCalculator::SubgridValWgtCalculator(const int nx, const int ny)
{
  for(int m = 0; m < ny; m++)
    {
      for (int l = 0; l < nx; l++)
	{
	  double area = pMin(l, nx - 1 - l ) * pMin(m, ny - 1 -m );
	  
	  if ( fabs(area - 0) < convWx::EPSILON)
	  {
	    area = minArea;
	  }
	  
	  pArea.push_back(area);	  
	}
    }
}

SubgridValWgtCalculator::~SubgridValWgtCalculator()
{
  pArea.erase(pArea.begin(),pArea.end());
}

double SubgridValWgtCalculator::get(const int i)const
{
  if ( static_cast<int>(pArea.size()) > i)
  {
    return pArea[i];
  }
  else
  {
    return noArea;
  }
}
