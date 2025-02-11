#include "gtest/gtest.h"

#include "h5zsperr_helper.h"

namespace {

TEST(h5zsperr_helper, pack_extra_info)
{
  // Test all possible combinations
  int rank = 0, is_float = 0, missing_val_mode = 0, magic = 0;
  for (int r = 2; r <= 3; r++)
    for (int f = 0; f <= 1; f++)
      for (int m = 0; m <= 2; m++)
        for (int g = 0; g <= 63; g++) {
          auto encode = C_API::h5zsperr_pack_extra_info(r, f, m, g);
          rank = r * f * m + 10;
          is_float = rank + 10;
          missing_val_mode = rank + 20;
          magic = rank - 20;
          C_API::h5zsperr_unpack_extra_info(encode, &rank, &is_float, &missing_val_mode, &magic);
          ASSERT_EQ(rank, r);
          ASSERT_EQ(is_float, f);
          ASSERT_EQ(missing_val_mode, m);
          ASSERT_EQ(magic, g);
      }
}

}
