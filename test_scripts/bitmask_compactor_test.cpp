#include "gtest/gtest.h"
#include <vector>
#include <limits>

extern "C" {
#include "bitmask_compactor.h"
}

namespace {

TEST(bitcpt, freq) {
  // Create an array of all zeros
  size_t N = 32;
  auto buf = std::vector<unsigned int>(N, 0); 
  auto ans = bitcpt_freq(buf.data(), N * sizeof(unsigned int)); 
  EXPECT_EQ(ans, 0);

  // Make the array of all ones 
  buf.assign(N, std::numeric_limits<unsigned int>::max());
  ans = bitcpt_freq(buf.data(), N * sizeof(unsigned int)); 
  EXPECT_EQ(ans, 1);

  // Make the array half half
  for (int i = 0; i < N / 2; i++)
    buf[i] = 0;
  ans = bitcpt_freq(buf.data(), N * sizeof(unsigned int)); 
  EXPECT_EQ(ans, 0);
}

} // End of the namespace

