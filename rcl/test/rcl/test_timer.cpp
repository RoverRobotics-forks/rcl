// Copyright 2016 Open Source Robotics Foundation, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <gtest/gtest.h>

#include <inttypes.h>

#include <chrono>
#include <thread>

#include "../scope_exit.hpp"
#include "rcl/error_handling.h"
#include "rcl/timer.h"

#include "../memory_tools/memory_tools.hpp"

#ifdef RMW_IMPLEMENTATION
# define CLASSNAME_(NAME, SUFFIX) NAME ## __ ## SUFFIX
# define CLASSNAME(NAME, SUFFIX) CLASSNAME_(NAME, SUFFIX)
#else
# define CLASSNAME(NAME, SUFFIX) NAME
#endif


#define TOLERANCE RCL_US_TO_NS(50)

TEST(CLASSNAME(TimerTestFixture, RMW_IMPLEMENTATION), test_timer_ready) {
  // Make a timer and check that it becomes ready after waiting
  rcl_timer_t timer = rcl_get_zero_initialized_timer();
  rcl_ret_t ret = rcl_timer_init(&timer, RCL_MS_TO_NS(1), nullptr, rcl_get_default_allocator());
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string_safe();

  rcl_time_point_value_t start = 0;
  rcl_time_point_value_t end = 0;

  // Check that the time until next call is close to the period
  int64_t time_until_call = INT64_MAX;
  ret = rcl_timer_get_time_until_next_call(&timer, &time_until_call);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string_safe();
  EXPECT_LE(RCL_MS_TO_NS(1) - time_until_call, TOLERANCE);

  ret = rcl_steady_time_now(&start);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string_safe();

  // Make sure we've slept
  do {
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string_safe();
    std::this_thread::sleep_for(std::chrono::microseconds(50));
    ret = rcl_steady_time_now(&end);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string_safe();
  } while (end - start < RCL_MS_TO_NS(1));

  bool is_ready = false;
  ret = rcl_timer_is_ready(&timer, &is_ready);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string_safe();
  EXPECT_TRUE(is_ready);

  ret = rcl_timer_fini(&timer);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string_safe();
}
