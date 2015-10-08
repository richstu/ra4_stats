#ifndef H_YIELD_KEY
#define H_YIELD_KEY

#include <tuple>
#include <utility>
#include <ostream>

#include "bin.hpp"
#include "process.hpp"
#include "cut.hpp"

//using YieldKey = decltype(std::forward_as_tuple(std::declval<Bin>(), std::declval<Process>(), std::declval<Cut>()));
using YieldKey = std::tuple<const Bin&, const Process&, const Cut&>;

const Bin & GetBin(const YieldKey &yk);
const Process & GetProcess(const YieldKey &yk);
const Cut & GetCut(const YieldKey &yk);

std::ostream & operator<<(std::ostream &stream, const YieldKey &key);

#endif
