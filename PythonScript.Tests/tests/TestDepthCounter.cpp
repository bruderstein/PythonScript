#include "stdafx.h"


#include <gtest/gtest.h>
#include "DepthCounter.h"

namespace NppPythonScript 
{
	
class DepthCounterTest : public ::testing::Test {
    virtual void SetUp() {

	}
};

TEST_F(DepthCounterTest, testInitialStateIsZero) {
    DepthCounter depthCounter;

    int depth = depthCounter.getDepth();

    ASSERT_EQ(depth, 0);
}

TEST_F(DepthCounterTest, testIncreaseIncreasesDepth) {
    DepthCounter depthCounter;

    DepthLevel depthLevel = depthCounter.increase();

    int depth = depthCounter.getDepth();

    ASSERT_EQ(depth, 1);
}

TEST_F(DepthCounterTest, testDestructorReturnsDepthToZero) {
    DepthCounter depthCounter;

    int insideBlockDepth, outsideBlockDepth;
    
    {
        DepthLevel depthLevel = depthCounter.increase();
        insideBlockDepth = depthCounter.getDepth();
    } // call depthLevel destructors

    outsideBlockDepth = depthCounter.getDepth();

    EXPECT_EQ(insideBlockDepth, 1);
    ASSERT_EQ(outsideBlockDepth, 0);
}

TEST_F(DepthCounterTest, testMultipleDepth) {
    DepthCounter depthCounter;

    int insideBlock1, insideBlock2;
    int outsideBlock1, outsideBlock2;

    {
        DepthLevel depthLevel = depthCounter.increase();
        insideBlock1 = depthCounter.getDepth();

        {
            DepthLevel depthLevel2 = depthCounter.increase();
            insideBlock2 = depthCounter.getDepth();
        }
        outsideBlock2 = depthCounter.getDepth();

    }
    outsideBlock1 = depthCounter.getDepth();

    EXPECT_EQ(insideBlock1, 1);
    EXPECT_EQ(insideBlock2, 2);
    EXPECT_EQ(outsideBlock2, 1);
    EXPECT_EQ(outsideBlock1, 0);
}


TEST_F(DepthCounterTest, testAssignment) {
    DepthCounter depthCounter;
    int insideBlock, outsideBlock;
    
	// As the DepthLevel is declared outside the block, we should have the same depth inside and outside the block.
    
	DepthLevel depthLevel;
	{
        depthLevel = depthCounter.increase();
        insideBlock = depthCounter.getDepth();
	}

    outsideBlock = depthCounter.getDepth();

    ASSERT_EQ(insideBlock, 1);
    ASSERT_EQ(outsideBlock, 1);
}

}