#include "artdaq-demo/Overlays/FragmentType.hh"

#include <algorithm>
#include <cassert>
#include <string>
#include <vector>

namespace {
  static std::vector<std::string> const
  names { "MISSED", "V1495" "V1720", "V1724", "V1190" };
}

demo::FragmentType
demo::toFragmentType(std::string t_string)
{
  std::transform(t_string.begin(),
                 t_string.end(),
                 t_string.begin(),
                 toupper);
  auto it = std::find(names.begin(), names.end(), t_string);
  return FragmentType::MISSED +
    (it == names.end()) ? static_cast<FragmentType>(0) : static_cast<FragmentType>(it - names.begin());
}

std::string
demo::fragmentTypeToString(FragmentType val)
{
  assert(val >=0 && val < FragmentType::INVALID);
  return names[val - FragmentType::MISSED];
}
