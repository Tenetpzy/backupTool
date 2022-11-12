#include "utils/lru_counter.h"
#include <gtest/gtest.h>
#include <iostream>

TEST(lru_counter_test, simple)
{
    lru_counter<int> lru;
    lru.add_element(1);
    lru.add_element(2);
    for (int i = 0; i < 3; ++i)
        lru.add_element_count(1);
    for (int i = 0; i < 2; ++i)
        lru.add_element_count(2);
    int less = lru.remove_less_use_entry();
    EXPECT_EQ(less, 2) << "less use test failed\n";
}

TEST(lru_counter_test, multiInsert)
{
    lru_counter<int> lru;
    
    for (int i = 1; i < 1000; ++i)
    {
        lru.add_element(i);
    }
    for (int i = 999; i >= 1; --i)
    {
        for (int j = 1000 - i; j > 0; --j)
            lru.add_element_count(i);
    }
    for (int i = 999; i >=1 ; --i)
        EXPECT_EQ(lru.remove_less_use_entry(), i);
}

TEST(lru_counter_test, forward_increase)
{
    lru_counter<int> lru;
    for (int i = 0; i < 10; ++i)
        lru.add_element(i);
    for (int i = 0; i < 10; ++i)
        for (int j = 0; j < 2; ++j)
            lru.add_element_count(i);
    for (int i = 0; i < 10; ++i)
        EXPECT_EQ(lru.remove_less_use_entry(), i);
}

TEST(lru_counter_test, backward_increase)
{
    lru_counter<int> lru;
    for (int i = 0; i < 10; ++i)
        lru.add_element(i);
    for (int i = 9; i >= 0; --i)
        for (int j = 0; j < 2; ++j)
            lru.add_element_count(i);
    for (int i = 0; i < 10; ++i)
        EXPECT_EQ(lru.remove_less_use_entry(), 9 - i);
}