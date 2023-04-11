/**
 * @file PcStats.cc
 */
#include <ConvWx/PcStats.hh>
using std::vector;

PcStats::PcStats(void)
{
  pNumWithPc = 0;
  pNumWithoutPc = 0;
}

PcStats::~PcStats(void)
{
}

void PcStats::print(void) const
{
  printf("PC:%d   NoPC:%d\n", pNumWithPc, pNumWithoutPc);
  for (size_t i=0; i<pStats.size(); ++i)
  {
    pStats[i].print();
  }
}

void PcStats::update(const PcStats1 &stats)
{
  if (stats.pNumRefineTested > 0)
  {
    pStats.push_back(stats);
    pNumWithPc++;
  }
  else
  {
    pNumWithoutPc++;
  }
}

void PcStats::merge(const PcStats &stats)
{
  pNumWithPc += stats.pNumWithPc;
  pNumWithoutPc += stats.pNumWithoutPc;
  for (size_t i=0; i<stats.pStats.size(); ++i)
  {
    pStats.push_back(stats.pStats[i]);
  }
}
