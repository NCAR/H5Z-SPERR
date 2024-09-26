#include "gtest/gtest.h"
#include <vector>
#include <memory>
#include <random>

extern "C" {
#include "h5z-bitstream.h"
}

namespace {


TEST(bitstream, StreamWriteRead)
{
  const size_t N = 159;
  auto mem = std::make_unique<uint64_t[]>(4);
  auto s1 = icecream();
  icecream_use_mem(&s1, mem.get(), 4);
  auto vec = std::vector<bool>(N);

  // Make N writes
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<unsigned int> distrib1(0, 1);
  for (size_t i = 0; i < N; i++) {
    const auto bit = distrib1(gen);
    vec[i] = bit;
    icecream_wbit(&s1, bit);
  }
  EXPECT_EQ(icecream_wtell(&s1), N);
  icecream_flush(&s1);
  EXPECT_EQ(icecream_wtell(&s1), 192);

  icecream_rewind(&s1);
  for (size_t i = 0; i < N; i++)
    EXPECT_EQ(icecream_rbit(&s1), vec[i]) << " at idx = " << i;
}


}
