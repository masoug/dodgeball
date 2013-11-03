/*
 * Main source file for dodgeball game.
 *
 * Copyright 2013 Sammy Guo
 */

#include <iostream>
#include <string>

#include "dodgeball_engine.h"

int main(int argc, char **argv) {
    std::cout << "D O D G E B A L L" << std::endl;

    /* server or client mode? */
    bool serverMode = false;
    std::string server = "localhost";
    if (argc > 1) {
        for (int i = 0; i < argc; i++) {
            if (strcmp(argv[i], "--server") == 0) {
                serverMode = true;
            } else if (strcmp(argv[i], "--serverAddress") == 0) {
                if (i < argc - 1) {
                    server = argv[i+1];
                } else {
                    std::cerr << "Please specify server address!" << std::endl;
                    return EXIT_FAILURE;
                }
            }
        } /* end for loop */
    }

    /* Initialize the dodgeball engine. */
    DodgeballEngine engine(640, 480, serverMode);
    
    /* First ask the player for the network info */
    if (engine.setupNetwork(server)) {
        if (!serverMode) {
            /* Register the user with the server */
            std::string username;
            std::cout << "\n\nPlease enter a username (no spaces!):" << std::endl;
            std::cin >> username;
            while (!engine.registerUser(username)) {
                std::cout << "Player request failed :( Please try again." << std::endl;
                std::cin >> username;
            }
            std::cout << "Hi there " << username << "! Welcome to dodgeball!" << std::endl;
        }
        
        /* Then set up the scene */
        engine.setupScene();

        /* Enter the main simulation loop. */
        engine.run();
    } else {
        std::cerr << "Failed to set up networking! Check your settings!" << std::endl;
    }

    /* Once we're done... */
    //engine.clearScene();

    std::cout << "Bye bye!" << std::endl;

    return 0;
}


