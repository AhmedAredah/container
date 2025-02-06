#include <gtest/gtest.h>
#include "container.h"

TEST(ContainerTest, BasicTest) {
    ContainerCore::Container container("TEST001", ContainerCore::Container::twentyFT);
    EXPECT_EQ(container.getContainerID(), "TEST001");
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}