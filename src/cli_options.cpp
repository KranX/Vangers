//
// Created by nikita on 11/30/17.
//
#include <cstring>
#include <iostream>
#include "cli_options.h"

void CommandLineOptions::parseArgs(int argc, char **argv) {
    for(int i = 1; i < argc; i++){
        if(strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0){
            printUsage();
            exit(0);
        }

        if(strcmp(argv[i], "--screen-width") == 0){
            if(i == argc - 1){
                std::cerr<<"Option \"--screen-width\" requires value"<<std::endl;
                printUsage();
                exit(1);
            }
            // TODO: int format errors
            screenWidth = atoi(argv[++i]);
            continue;
        }

        if(strcmp(argv[i], "--screen-height") == 0){
            if(i == argc - 1){
                std::cerr<<"Option \"--screen-height\" requires value"<<std::endl;
                printUsage();
                exit(1);
            }
            // TODO: int format errors
            screenHeight = atoi(argv[++i]);
            continue;
        }
        std::cerr<<"Unknown option: \""<<argv[i]<<"\""<<std::endl;
        printUsage();
        exit(1);
    }
}

void CommandLineOptions::printUsage() {
    const char *usage =
                "Usage:\n"
                 "-h, --help   Show this help message and exit\n"
                "--screen-widht <screenWidth>\n"
                "--screen-height <screenHeight>\n"
        "";
    std::cout<<usage;

}

int CommandLineOptions::getScreenWidth() const {
    return screenWidth;
}

int CommandLineOptions::getScreenHeight() const {
    return screenHeight;
}

CommandLineOptions::CommandLineOptions() : screenWidth(800),
                                           screenHeight(600) {}
