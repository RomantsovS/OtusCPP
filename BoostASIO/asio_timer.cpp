// #define BOOST_ASIO_ENABLE_HANDLER_TRACKING
// https://www.boost.org/doc/libs/1_66_0/doc/html/boost_asio/overview/core/handler_tracking.html

#include <boost/asio/io_service.hpp>
#include <boost/asio/steady_timer.hpp>
#include <chrono>
#include <ctime>
#include <iostream>
#include <thread>

void print_time() {
    using clock = std::chrono::system_clock;

    auto time_point = clock::now();
    std::time_t time = clock::to_time_t(time_point);
    std::cout << std::ctime(&time) << std::endl;
}

void test_timer_sync() {
    std::cout << "\ntest_timer_sync\n" << std::endl;

    // Old style
    // using boost::asio::io_service = boost::asio::io_context;

    boost::asio::io_context io_context;
    boost::asio::steady_timer timer{io_context, std::chrono::seconds{1}};

    std::cout << "Before call timer.wait()" << std::endl;
    print_time();

    // Erorr-code based interface
    boost::system::error_code errCode;
    // Block here for 1 second
    timer.wait(errCode);
    if (errCode) {
        std::cout << "Error! " << errCode.message() << std::endl;
    }

    std::cout << "Before call timer.wait() second time" << std::endl;
    print_time();
    try {
        // Block here (but not for the second time)
        timer.wait();
    } catch (const boost::system::system_error& ex) {
        std::cout << "Exception! " << ex.what();
    }

    std::cout << "After timeout!" << std::endl;
    print_time();

    std::cout << std::endl;
}

void test_timer_async() {
    std::cout << "\ntest_timer_async\n" << std::endl;

    boost::asio::io_context io_context;
    boost::asio::steady_timer timer{io_context, std::chrono::seconds{1}};
    std::cout << "async_wait" << std::endl;
    print_time();
    // Non-blocking call
    timer.async_wait([](const boost::system::error_code& error) {
        // boost::asio::error::operation_aborted is a possible error here
        if (errno)
            std::cout << "Error! " << error.message() << std::endl;
        else
            std::cout << "Hello from async timeout!" << std::endl;
    });

    std::cout << "before io_context.run" << std::endl;
    print_time();

    // boost::asio::io_context::work work(io_context);
    // Block here
    io_context.run();
    std::cout << "after io_context.run" << std::endl;
    print_time();

    std::cout << std::endl;
}

void test_two_timers_async() {
    std::cout << "\ntest_two_timers_async\n" << std::endl;

    boost::asio::io_context io_context;

    print_time();

    boost::asio::steady_timer timer1{io_context, std::chrono::seconds{1}};
    // Non-blocking call here
    timer1.async_wait([](const boost::system::error_code&) {
        std::cout << "Hello from async timeout1!" << std::endl;
        print_time();
    });

    boost::asio::steady_timer timer2{io_context, std::chrono::seconds{1}};
    // Non-blocking call here
    timer2.async_wait([](const boost::system::error_code&) {
        std::cout << "Hello from async timeout2!" << std::endl;
        print_time();
    });

    std::cout << "before io_context.run" << std::endl;
    print_time();
    // Block here
    io_context.run();
    std::cout << "after io_context.run" << std::endl;
    print_time();

    std::cout << std::endl;
}

void test_timer_async_wich_thread() {
    std::cout << "\ntest_timer_async_wich_thread\n" << std::endl;

    boost::asio::io_context io_context;
    boost::asio::steady_timer timer{io_context, std::chrono::seconds{1}};
    timer.async_wait([](const boost::system::error_code&) {
        std::cout << std::this_thread::get_id() << " Hello from async timeout!" << std::endl;
    });

    std::cout << std::this_thread::get_id() << " io_context.run" << std::endl;
    io_context.run();

    std::cout << std::endl;
}

void timeoutFunction(const boost::system::error_code&, int value) {
    std::cout << "Hello from timeoutFunction with value = " << value << std::endl;
}

void test_timer_async_args() {
    std::cout << "\ntest_timer_async_args\n" << std::endl;

    boost::asio::io_context io_context;
    boost::asio::steady_timer timer{io_context, std::chrono::seconds{1}};

    int arg = 42;
    timer.async_wait(
        // Local variable!
        [&arg](const boost::system::error_code& err) { timeoutFunction(err, arg); });

    io_context.run();

    std::cout << std::endl;
}

void test_two_threads() {
    std::cout << "\ntest_two_threads\n" << std::endl;

    boost::asio::io_context io_context;

    boost::asio::steady_timer timer1{io_context, std::chrono::seconds{1}};
    timer1.async_wait([](const boost::system::error_code&) {
        std::cout << std::this_thread::get_id() << " Hello from async timeout1!" << std::endl;
    });

    boost::asio::steady_timer timer2{io_context, std::chrono::seconds{1}};
    timer2.async_wait([](const boost::system::error_code&) {
        std::cout << std::this_thread::get_id() << " Hello from async timeout2!" << std::endl;
    });

    // Yes, we can post custom task to the io_context
    // Post - async execution
    io_context.post([]() {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << std::this_thread::get_id() << " Hello from posted function!" << std::endl;
    });

    // Dispatch - no async guarantees
    io_context.dispatch([]() {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << std::this_thread::get_id() << " Hello from dispatched function!" << std::endl;
    });

    std::thread thread1{[&io_context]() { io_context.run(); }};
    std::thread thread2{[&io_context]() { io_context.run(); }};

    thread1.join();
    thread2.join();

    std::cout << std::endl;
}

int main() {
    test_timer_sync();
    test_timer_async();
    test_two_timers_async();
    test_timer_async_wich_thread();
    test_timer_async_args();
    test_two_threads();

    return 0;
}
