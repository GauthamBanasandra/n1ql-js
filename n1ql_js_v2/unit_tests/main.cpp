//
//  main.cpp
//  unit_tests
//
//  Created by Gautham Banasandra on 07/03/17.
//  Copyright © 2017 Couchbase. All rights reserved.
//
#include "gtest/gtest.h"

using namespace std;

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
