#ifndef ds50daq_StatsKeeper_h
#define ds50daq_StatsKeeper_h

#include <iostream>
#include <vector>
#include <math.h>

namespace ds50
{

  class StatsKeeper
  {
  public:
    StatsKeeper():
      bins_(200),
      mean_(0),
      m2_(0),
      n_(0)
    { }

    void put(size_t orig_size, size_t comp_size)
    {
      double ratio = (double(comp_size) / double(orig_size));
      size_t bin = (size_t)floor(ratio*100.);
      ++bins_[ bin<200 ? bin : 199];

      n_ += 1;
      double delta = ratio-mean_;
      mean_ += (delta / n_);
      m2_ += (delta * (ratio - mean_));
    }

    std::vector<size_t> const& bins() const { return bins_; }
    double mean() const { return mean_; }
    double s() const { return sqrt(m2_/(n_-1)); }
    void print(std::ostream& ost) const;
  private:
    std::vector<size_t> bins_;
    double mean_;
    double m2_;
    size_t n_;
  };

inline void StatsKeeper::print(std::ostream& ost) const
{
  ost << "mean=" << mean() << ", " << "std=" << s() << "\n";
}

inline std::ostream& operator<<(std::ostream& ost, StatsKeeper const& s)
{
  s.print(ost);
  return ost;
}

}

#endif
