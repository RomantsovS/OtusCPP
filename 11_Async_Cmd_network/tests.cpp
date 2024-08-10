#include <gtest/gtest.h>

#include <sstream>
#include <thread>

#include "Client.h"
#include "Server.h"

TEST(TestAll, Test1) {
    {
        std::ostringstream os;
        const std::string expected = R"(bulk: cmd1, cmd2, cmd3
bulk: cmd4, cmd5
)";

        BlockProcessor bp(3);
        BlockPrinter bpr(os, &bp);
        // BlockSaver bs(&bp, 1);

        boost::asio::io_context io_context;

        Server server(io_context, 7900, bp);

        std::thread io_thread([&io_context] { io_context.run(); });

        std::thread([] {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            ba::io_context io_context_client;

            std::vector<std::string> str = {"cmd1\n", "cmd2\n", "cmd3\n", "cmd4\n", "cmd5\n"};

            AsyncClient client(io_context_client, 7900, std::move(str));
            io_context_client.run();
        }).join();

        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // to receive commands
        bp.Stop();
        io_context.stop();
        io_thread.join();
        bpr.Stop();

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

        BlockProcessor bp(3);
        BlockPrinter bpr(os, &bp);
        // BlockSaver bs(&bp, 1);

        boost::asio::io_context io_context;

        Server server(io_context, 7901, bp);

        std::thread io_thread([&io_context] { io_context.run(); });

        std::thread([] {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            ba::io_context io_context_client;

            std::vector<std::string> str = {R"(cmd1
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
cmd11)"};

            AsyncClient client(io_context_client, 7901, std::move(str));
            io_context_client.run();
        }).join();

        std::this_thread::sleep_for(std::chrono::seconds(1)); // to receive commands
        bp.Stop();
        io_context.stop();
        io_thread.join();
        bpr.Stop();

        EXPECT_EQ(os.str(), expected);
    }
}

TEST(TestAll, Test3) {
    {
        std::ostringstream os;
        BlockProcessor bp(3);
        BlockPrinter bpr(os, &bp);
        // BlockSaver bs(&bp, 1);

        boost::asio::io_context io_context;

        Server server(io_context, 7902, bp);

        std::thread io_thread([&io_context] { io_context.run(); });

        std::thread([] {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            ba::io_context io_context_client;

            std::vector<std::string> str = {"0\n", "1\n", "2\n", "3\n", "4\n", "5\n"};

            AsyncClient client(io_context_client, 7902, std::move(str));
            io_context_client.run();
        }).detach();

        std::thread([] {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            ba::io_context io_context_client;

            std::vector<std::string> str = {"10\n", "11\n", "12\n", "13\n", "14\n", "15\n"};

            AsyncClient client(io_context_client, 7902, std::move(str));
            io_context_client.run();
        }).detach();

        std::this_thread::sleep_for(std::chrono::seconds(3));
        bp.Stop();
        io_context.stop();
        io_thread.join();
        bpr.Stop();

        auto str = os.str();
        std::cout << str;
        EXPECT_EQ(str.size(), 62);
    }
}