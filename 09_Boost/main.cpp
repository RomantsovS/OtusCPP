#include <boost/program_options.hpp>
#include <iostream>
#include <string>

#include "FileScanner.h"
#include "FileSystem.h"

namespace po = boost::program_options;

int main(int argc, char const* argv[]) {
    try {
        po::options_description desc{"Options"};
        desc.add_options()("help,h", "help")(
            "dir-exclusion",
            po::value<std::vector<std::filesystem::path>>()->default_value(std::vector<std::filesystem::path>(), ""),
            "dir exclusion")("scan-level", po::value<int>()->default_value(0), "scan level (0/1)")(
            "min-size", po::value<size_t>()->default_value(1), "min size")(
            "mask", po::value<std::string>()->default_value(""), "mask")(
            "block-size", po::value<size_t>()->default_value(100), "block size")(
            "input-dir", po::value<std::vector<std::filesystem::path>>(), "input dir");

        po::positional_options_description p;
        p.add("input-dir", -1);

        po::variables_map vm;
        po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
        po::notify(vm);

        if (vm.count("help")) {
            std::cout << desc << '\n';
            return 0;
        }

        // for(const auto& val : vm["input-dir"].as<std::vector<std::filesystem::path>>()) {
        //     std::cout << val.u8string() << std::endl;
        // }

        FileSystem fs(vm["input-dir"].as<std::vector<std::filesystem::path>>(), vm["mask"].as<std::string>(),
                      vm["dir-exclusion"].as<std::vector<std::filesystem::path>>(), vm["min-size"].as<size_t>());
        FileScanner scanner(std::move(fs.ListFiles(vm["scan-level"].as<int>() != 0)), vm["block-size"].as<size_t>());
        scanner.Scan();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}