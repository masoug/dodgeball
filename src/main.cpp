/*
 * Main source file for dodgeball game.
 *
 * Copyright 2013 Sammy Guo
 */

#include <iostream>

#include "dodgeball_engine.h"

int main(int argc, char **argv) {
    std::cout << "D O D G E B A L L" << std::endl;

    /* Initialize the dodgeball engine. */
    DodgeballEngine engine(640, 480);
    
    /* First ask the player for the network info */
    if (engine.setupNetwork()) {
        
        /* Then set up the scene */
        engine.setupScene();

        /* Enter the main simulation loop. */
        engine.run();
    }

    /* Once we're done... */
    //engine.clearScene();

    std::cout << "Bye bye!" << std::endl;

    return 0;
}


