#include <gtest/gtest.h>

#include <sstream>

#include "processing.h"

TEST(TestAll, TestStaticBlock) {
    {
        std::istringstream is("cmd1\ncmd2\ncmd3\ncmd4\ncmd5");
        std::ostringstream os;

        const std::string expected = R"(bulk: cmd1, cmd2, cmd3
bulk: cmd4, cmd5
)";

        BlockProcessor bp(3);
        BlockPrinter bpr(&bp, os);
        BlockSaver bs(&bp);

        bp.RunCmds(is);

        EXPECT_EQ(os.str(), expected);
    }
}

TEST(TestAll, TestDynamicBlock) {
    {
        std::istringstream is(R"(cmd1
cmd2
{
cmd3
cmd4
}
{
cmd5
cmd6
{
cmd7
cmd8
}
cmd9
}
{
cmd10
cmd11
)");
        std::ostringstream os;

        const std::string expected = R"(bulk: cmd1, cmd2
bulk: cmd3, cmd4
bulk: cmd5, cmd6, cmd7, cmd8, cmd9
)";

        BlockProcessor bp(3);
        BlockPrinter bpr(&bp, os);
        BlockSaver bs(&bp);

        bp.RunCmds(is);

        EXPECT_EQ(os.str(), expected);
    }
}

TEST(TestAll, TestDynamicBlock2) {
    {
        std::istringstream is(R"(cmd1
cmd2
{
cmd3
cmd4
}
cmd5
cmd6
)");
        std::ostringstream os;

        const std::string expected = R"(bulk: cmd1, cmd2
bulk: cmd3, cmd4
bulk: cmd5, cmd6
)";

        {
            BlockProcessor bp(3);
            BlockPrinter bpr(&bp, os);
            BlockSaver bs(&bp);

            bp.RunCmds(is);
        }

        EXPECT_EQ(os.str(), expected);
    }
}