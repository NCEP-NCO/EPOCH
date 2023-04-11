/**
 * @file TileThreshInfoGenBased.cc
 */
#include <Epoch/TileThreshInfoGenBased.hh>
#include <Epoch/SingleTileThresholdsGenBased.hh>
#include <toolsa/DateTime.hh>
#include <toolsa/LogStream.hh>

//-------------------------------------------------------------------
TileThreshInfoGenBased::
TileThreshInfoGenBased(const SingleTileThresholdsGenBased &mthresh,
		       const time_t &genTime,
		       int leadTime, int tileIndex):
  _ok(true),
  _thresh(mthresh.getThresh()),
  _bias(mthresh.getBias()),
  _coldstart(mthresh.getColdstart()),
  _motherTile(mthresh.getMotherTile()),
  _genTime(genTime), _leadTime(leadTime), _tileIndex(tileIndex)
{
}

//-------------------------------------------------------------------
void TileThreshInfoGenBased::print(void) const
 {
   printf("%s Gen:%s+%08d", _field.c_str(),
	  DateTime::strn(_genTime).c_str(), _leadTime);
   printf(" tile:%3d THRESH:%8.2lf bias:%10.8lf ",
	  _tileIndex, _thresh, _bias);
  if (_motherTile)
  {
    printf("Mother ");
  }
  else
  {
    printf("       ");
  }
  if (_coldstart)
  {
    printf("Coldstart\n");
   }
  else
  {
    printf("\n");
  }
}

