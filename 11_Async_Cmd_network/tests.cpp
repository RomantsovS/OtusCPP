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
        BlockSaver bs(&bp, 1);

        boost::asio::io_context io_context;

        Server server(io_context, 7900, bp);

        std::thread([&io_context] { io_context.run(); }).detach();

        std::thread([] {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            ba::io_context io_context_client;

            std::vector<std::string> str = {"cmd1\ncmd2\ncmd3\ncmd4\ncmd5"};

            AsyncClient client(io_context_client, 7900, std::move(str));
            io_context_client.run();
        }).join();

        bp.Stop();
        std::this_thread::sleep_for(std::chrono::seconds(3));

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
        BlockSaver bs(&bp, 1);

        boost::asio::io_context io_context;

        Server server(io_context, 7901, bp);

        std::thread([&io_context] { io_context.run(); }).detach();

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

        bp.Stop();

        EXPECT_EQ(os.str(), expected);
    }
}

TEST(TestAll, Test3) {
    {
        BlockProcessor bp(3);
        BlockPrinter bpr(std::cout, &bp);
        BlockSaver bs(&bp, 1);

        boost::asio::io_context io_context;

        Server server(io_context, 7902, bp);

        std::thread([&io_context] { io_context.run(); }).detach();

        std::thread([] {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            ba::io_context io_context_client;

            std::vector<std::string> str = {"0", "1", "2", "3", "4", "5"};

            AsyncClient client(io_context_client, 7902, std::move(str));
            io_context_client.run();
        }).detach();

        std::thread([] {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            ba::io_context io_context_client;

            std::vector<std::string> str = {"10", "11", "12", "13", "14", "15"};

            AsyncClient client(io_context_client, 7902, std::move(str));
            io_context_client.run();
        }).detach();

        std::this_thread::sleep_for(std::chrono::seconds(10));

        bp.Stop();
    }
}