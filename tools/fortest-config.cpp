#include "FortestConfig.h"
#include <iostream>
#include <string>

void print_help() {
    std::cout << "Usage: fortest-config [OPTION]\n\n"
              << "Options:\n"
              << "  --prefix         Installation prefix\n"
              << "  --includedir     C/C++ include directory\n"
              << "  --moddir         Fortran module directory\n"
              << "  --libdir         Library directory\n"
              << "  --cmake-prefix   CMake package config directory\n"
              << "  --libs           Link flags for Fortest (-L... -lfortest)\n"
              << "  --version        Fortest version\n"
              << "  --all            Print all configuration information\n"
              << "  --help           Show this help message\n";
}

void print_all() {
    std::cout << "Fortest configuration:" << std::endl;
    std::cout << "  version:       " << FORTEST_VERSION << std::endl;
    std::cout << "  prefix:        " << FORTEST_PREFIX << std::endl;
    std::cout << "  include dir:   " << FORTEST_INCLUDEDIR << std::endl;
    std::cout << "  module dir:    " << FORTEST_MODDIR << std::endl;
    std::cout << "  library dir:   " << FORTEST_LIBDIR << std::endl;
    std::cout << "  cmake prefix:  " << FORTEST_CMAKEPREFIX << std::endl;
    std::cout << "  libs:          -L" << FORTEST_LIBDIR << " -lfortest" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_help();
        return 1;
    }

    std::string arg = argv[1];

    if (arg == "--prefix") {
        std::cout << FORTEST_PREFIX << std::endl;
    } else if (arg == "--includedir") {
        std::cout << FORTEST_INCLUDEDIR << std::endl;
    } else if (arg == "--moddir") {
        std::cout << FORTEST_MODDIR << std::endl;
    } else if (arg == "--libdir") {
        std::cout << FORTEST_LIBDIR << std::endl;
    } else if (arg == "--cmake-prefix") {
        std::cout << FORTEST_CMAKEPREFIX << std::endl;
    } else if (arg == "--libs") {
        std::cout << "-L" << FORTEST_LIBDIR << " -lfortest" << std::endl;
    } else if (arg == "--version") {
        std::cout << FORTEST_VERSION << std::endl;
    } else if (arg == "--all") {
        print_all();
    } else if (arg == "--help" || arg == "-h") {
        print_help();
    } else {
        std::cerr << "Unknown option: " << arg << "\n\n";
        print_help();
        return 1;
    }

    return 0;
}
