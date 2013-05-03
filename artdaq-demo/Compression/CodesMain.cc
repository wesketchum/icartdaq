
#include <fstream>
#include <iostream>
#include <algorithm>
#include <string>
#include <math.h>

#include "artdaq-demo/Compression/Properties.hh"
#include "artdaq-demo/Compression/SymTable.hh"
#include "artdaq-demo/Compression/Codes.hh"

using namespace demo;
using namespace std;

void readTrainingSet(istream & ifs, ADCCountVec & out, size_t max_samples)
{
  bool forever = max_samples == 0 ? true : false;
  const size_t sz = sizeof(adc_type);
  while (forever || max_samples > 0) {
    adc_type data;
    ifs.read((char *)&data, sz);
    if (ifs.eof()) { break; }
    out.push_back(data);
    --max_samples;
  }
}

// ------------------------------

int main(int argc, char * argv[])
{
  if (argc < 2) {
    cerr << "Usage: "
         << argv[0]
         << " max_codes\n";
    return -1;
  }
  size_t max_codes = atoi(argv[1]);
  SymTable pod_table, subexp0_table, subexp1_table;
  for (size_t i = 0; i < max_codes; ++i) {
    Code p = pod(i);
    Code s0 = subexp<0>(i);
    Code s1 = subexp<1>(i);
    pod_table.push_back(SymCode(i, p.value_, p.length_));
    subexp0_table.push_back(SymCode(i, s0.value_, s0.length_));
    subexp1_table.push_back(SymCode(i, s1.value_, s1.length_));
  }
  ::writeTable("pod_table.txt", pod_table);
  ::writeTable("subexp0_table.txt", subexp0_table);
  ::writeTable("subexp1_table.txt", subexp1_table);
  return 0;
}
