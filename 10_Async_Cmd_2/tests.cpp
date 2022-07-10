#include <gtest/gtest.h>

#include <sstream>

#include "async.h"
#include <future>

TEST(TestAll, Test1) {
    {
        std::ostringstream os;
        const std::string expected = R"(bulk: cmd1, cmd2, cmd3
bulk: cmd4, cmd5
)";

        {
            auto h = async::connect(3, os);
            std::string str = "cmd1\ncmd2\ncmd3\ncmd4\ncmd5";
            async::receive(h, str.c_str(), str.size());
            async::disconnect(h);
        }

        EXPECT_EQ(os.str(), expected);
    }
}

TEST(TestAll, Test2) {
    {
        std::ostringstream os;
        const std::string expected = R"(bulk: cmd1, cmd2
bulk: cmd3, cmd4
bulk: cmd5, cmd6, cmd7, cmd8, cmd9
)";

        {
            auto h = async::connect(3, os);

            std::string str = R"(cmd1
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
cmd11)";
            async::receive(h, str.c_str(), str.size());
            async::disconnect(h);
        }

        EXPECT_EQ(os.str(), expected);
    }
}

TEST(TestAll, Test3) {
    {
        std::ostringstream os;
        const std::string expected = R"(bulk: cmd1, cmd2
bulk: cmd3, cmd4
bulk: cmd5, cmd6
)";

        {
            auto h = async::connect(3, os);

            std::string str = R"(cmd1
cmd2
{
cmd3
cmd4
}
cmd5
cmd6
)";
            async::receive(h, str.c_str(), str.size());
            async::disconnect(h);
        }

        EXPECT_EQ(os.str(), expected);
    }
}

TEST(TestAll, Test4) {
    {
        std::ostringstream os;
        const std::string expected = R"(bulk: cmd1
)";

        {
            auto h = async::connect(5, os);
            async::receive(h, "cmd1", 4);
            async::disconnect(h);
        }

        EXPECT_EQ(os.str(), expected);
    }
}

TEST(TestAll, Test5) {
    {
        std::ostringstream os;
        const std::string expected = R"(bulk: cmd1, 2, 3, 4, 5
bulk: 6
)";

        {
            auto h = async::connect(5, os);
            async::receive(h, "cmd1", 4);
            async::receive(h, "\n2\n3\n4\n5\n6\n", 11);
            async::disconnect(h);
        }

        EXPECT_EQ(os.str(), expected);
    }
}

TEST(TestAll, Test6) {
    {
        std::ostringstream os;
        const std::string expected = R"(bulk: cmd1, 2, 3, 4, 5
bulk: 6
bulk: a, b, c, d
bulk: 89
)";

        {
            auto h = async::connect(5, os);
            async::receive(h, "cmd1", 4);
            async::receive(h, "\n2\n3\n4\n5\n6\n{\na\n", 15);
            async::receive(h, "b\nc\nd\n}\n89\n", 11);
            async::disconnect(h);
        }

        EXPECT_EQ(os.str(), expected);
    }
}

TEST(TestAll, Test7) {
    {
        std::ostringstream os1;
        std::ostringstream os2;
        const std::string expected1 = R"(bulk: cmd1
)";

        const std::string expected2 = R"(bulk: cmd2
)";

        {
            auto h = async::connect(5, os1);
            auto h2 = async::connect(5, os2);

            async::receive(h, "cmd1", 4);
            async::receive(h2, "cmd2", 4);
            async::disconnect(h);
            async::disconnect(h2);
        }

        EXPECT_EQ(os1.str(), expected1);
        EXPECT_EQ(os2.str(), expected2);
    }
}

TEST(TestAll, Test8) {
    {
        std::ostringstream os1;
        std::ostringstream os2;
        const std::string expected1 = R"(bulk: cmd1, 2, 3, 4, 5
)";

        const std::string expected2 = R"(bulk: cmd6, 7, 8, 9
)";

        {
            auto h = async::connect(5, os1);
            auto h2 = async::connect(5, os2);

            async::receive(h, "cmd1", 4);
            async::receive(h2, "cmd6", 4);
            async::receive(h, "\n2\n3\n4\n5\n", 9);
            async::receive(h2, "\n7\n8\n9\n", 7);
            
            async::disconnect(h);
            async::disconnect(h2);
        }

        EXPECT_EQ(os1.str(), expected1);
        EXPECT_EQ(os2.str(), expected2);
    }
}

TEST(TestAll, Test9) {
    {
        std::ostringstream os1;
        std::ostringstream os2;
        const std::string expected1 = R"(bulk: cmd1, 2, 3, 4, 5
bulk: 6
bulk: a, b, c, d
bulk: 89
)";

        const std::string expected2 = R"(bulk: cmd6, 7, 8, 9
)";

        auto f1 = std::async(std::launch::async, [&os1, &expected1](){
            auto h = async::connect(5, os1);

            async::receive(h, "cmd1", 4);
            async::receive(h, "\n2\n3\n4\n5\n6\n{\na\n", 15);
            async::receive(h, "b\nc\nd\n}\n89\n", 11);
            
            async::disconnect(h);
        });

        auto f2 = std::async(std::launch::async, [&os2, &expected2](){
            auto h2 = async::connect(5, os2);

            async::receive(h2, "cmd6", 4);
            async::receive(h2, "\n7\n8\n9\n", 7);
            
            async::disconnect(h2);
        });

        f1.get();
        f2.get();

        EXPECT_EQ(os1.str(), expected1);
        EXPECT_EQ(os2.str(), expected2);
    }
}