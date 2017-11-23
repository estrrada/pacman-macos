//
//  main.cpp
//  PAC-MAN 2017
//
//  Created by Carlos Estrada on 11/16/17.
//  Copyright © 2017 Carlos Estrada. All rights reserved.
//

#include "Game.hpp"

int main() {
    /*** WINDOW & CLOCKS ***/
    RenderWindow window(VideoMode(WinX, WinY), "PAC-MAN 2017", Style::Default);
    View view(Vector2f(WinX / 2, WinY / 2), Vector2f(WinX, WinY));
    window.setKeyRepeatEnabled(false);

    Clock deltaClock, edibleClock, fruitClock, startClock, deathClock;
    Time deltaTime, startTime, deathTime;
    float edibleTime, fruitTime;
    
    /*** VARIABLES ***/
    int ghostCount = 0, pelletCount = 0, lifeCount = 4, lifeScore = 10000;
    int gamestate = STARTING, level = 1, score = 0;
    float looppitch = 1.00, edibleLimit = 10.0;
    std::ostringstream ss;
    
    bool isEdible, isEaten, startInit, showFruit;
    isEdible = isEaten = startInit = showFruit = false;

    /*** MAZE ***/
    Texture MazeBG;
    MazeBG.loadFromFile(resourcePath() + "/Graphics/maze-bg.png");
    RectangleShape background(Vector2f(WinLength, WinHeight));
    background.setTexture(&MazeBG);

    MazeData maze;
    maze.loadPellets(4);  maze.placePellets(4);
    maze.loadPellets(240); maze.placePellets(240);
    maze.placeNodes(); maze.setValidNodeMovements();

    /*** TEXT ***/
    addText playerScore, scoreText("SCORE"), ready("READY!"), gameover("GAME   OVER");
    addText youwin("YOU WIN!"), playAgain("PRESS SPACE TO PLAY AGAIN"), continueLevel("PRESS SPACE TO CONTINUE");

    scoreText.setPosition(Vector2f(125, 40));
    playerScore.setPosition(Vector2f(250, 40));
    ready.setPosition(Vector2f(450, 550));
    gameover.setPosition(Vector2f(370, 540));
    youwin.setPosition(Vector2f(405, 540));
    playAgain.setPosition(Vector2f(475, 40));
    continueLevel.setPosition(Vector2f(326, 730));

    scoreText.setFillColor(Color(208, 62, 25));            // RED
    playerScore.setFillColor(Color(255, 255, 255));        // WHITE
    ready.setFillColor(Color(255, 255, 0));                // YELLOW
    gameover.setFillColor(Color(208, 62, 25));            // RED
    youwin.setFillColor(Color(70, 191, 238));            // CYAN
    playAgain.setFillColor(Color(255, 255, 0));            // YELLOW
    continueLevel.setFillColor(Color(255, 255, 255));    // WHITE

    ready.setCharacterSize(40);
    gameover.setCharacterSize(53);
    youwin.setCharacterSize(53);

    updatePoints(ss, score, playerScore);
    // END TEXT

    /*** PLAYERS ***/
    float pacmanSpeed = 200, ghostSpeed = 120;
    Vector2f pacmanPos(512, 746), blinkyPos(512, 405), pinkyPos(512, 501), inkyPos(463, 501), clydePos(562, 501);
    Player pacman("char-pacman.png", 16, 3, 20); pacman.setFrameTime(seconds(0.02));
    Player blinky("char-blinky.png", 16, 6, 2); blinky.setFrameTime(seconds(0.25));
    Player inky("char-inky.png", 16, 6, 2); inky.setFrameTime(seconds(0.25));
    Player pinky("char-pinky.png", 16, 6, 2); pinky.setFrameTime(seconds(0.25));
    Player clyde("char-clyde.png", 16, 6, 2); clyde.setFrameTime(seconds(0.25));
    
    float scale = 2.5;
    pacman.setScale(scale, scale);
    blinky.setScale(scale, scale);
    inky.setScale(scale, scale);
    pinky.setScale(scale, scale);
    clyde.setScale(scale, scale);
    
    pacman.setPlayerSpeed(pacmanSpeed);
    blinky.setPlayerSpeed(ghostSpeed);
    inky.setPlayerSpeed(ghostSpeed);
    pinky.setPlayerSpeed(ghostSpeed);
    clyde.setPlayerSpeed(ghostSpeed);

    resetGame(pacman, blinky, inky, pinky, clyde, maze, edibleTime, gamestate);
    // END PLAYERS

    /*** SOUNDS ***/
    Audio chomp1("chomp1.wav"), chomp2("chomp2.wav"), scatter("scatter.wav"), theme("theme.wav");
    Audio siren("siren.wav"), eatfruit("eatfruit.wav"), life("life.wav"), death("death.wav"), eatghost("eatghost.wav");
    scatter.setLoop(true); siren.setLoop(true); theme.play();
    // END SOUNDS

    /*** FRUIT ***/
    Player fruit("fruit-all.png", 16, 6, 1);
    fruit.setPosition(Vector2f(512, 576));
    fruit.setScale(scale, scale);

    RectangleShape fruitSprite[6];    // FRUIT LIST
    for (int i = 0; i < 6; i++) {
        fruitSprite[i].setSize(Vector2f(spriteSize, spriteSize));
        fruitSprite[i].setPosition(Vector2f(832 - ((spriteSize * 1.25) * i), 970));
    }

    Texture cherry, strawberry, orange, apple, watermelon, key;
    cherry.loadFromFile(resourcePath() + "/Graphics/fruit-cherry.png");
    strawberry.loadFromFile(resourcePath() + "/Graphics/fruit-strawberry.png");
    orange.loadFromFile(resourcePath() + "/Graphics/fruit-orange.png");
    apple.loadFromFile(resourcePath() + "/Graphics/fruit-apple.png");
    watermelon.loadFromFile(resourcePath() + "/Graphics/fruit-watermelon.png");
    key.loadFromFile(resourcePath() + "/Graphics/fruit-key.png");

    fruitSprite[0].setTexture(&cherry);
    fruitSprite[1].setTexture(&strawberry);
    fruitSprite[2].setTexture(&orange);
    fruitSprite[3].setTexture(&apple);
    fruitSprite[4].setTexture(&watermelon);
    fruitSprite[5].setTexture(&key);
    // END FRUIT

    RectangleShape lives[7];
    Texture livesTexture;
    livesTexture.loadFromFile(resourcePath() + "/Graphics/maze-lives.png");
    for (int i = 0; i < 7; i++) {
        lives[i].setSize(Vector2f(spriteSize, spriteSize));
        lives[i].setTexture(&livesTexture);
        lives[i].setPosition(Vector2f(150 + ((spriteSize * 1.25) * i), 970));
    }
    // END VARIABLES

    /*** PAUSE STATE ***/
    Texture pauseBG, selectArrow;
    pauseBG.loadFromFile(resourcePath() + "/Graphics/menu-bg.png");
    selectArrow.loadFromFile(resourcePath() + "/Graphics/menu-select.png");

    RectangleShape pauseOpacity, pauseMenu, select;
    Vector2f selectPos(380, 365);

    pauseOpacity.setSize(Vector2f(WinX, WinY));
    pauseMenu.setSize(Vector2f(600, 750));
    select.setSize(Vector2f(32, 32));

    pauseOpacity.setPosition(0, 0);
    pauseMenu.setPosition(Vector2f(512, 512));
    pauseMenu.setOrigin(pauseMenu.getSize().x / 2.0, pauseMenu.getSize().y / 2.0);
    select.setPosition(selectPos);

    pauseOpacity.setFillColor(Color(0, 0, 0, 100));
    pauseMenu.setTexture(&pauseBG);
    select.setTexture(&selectArrow);

    addText paused("PAUSED"), continueText("CONTINUE"), restartText("RESTART");
    addText quitText("QUIT"), controlText("CONTROLS"), WASDText("WASD KEYS >"), ArrowText("< ARROW KEYS");

    paused.setFillColor(Color(70, 191, 238));            // CYAN
    continueText.setFillColor(Color(255, 255, 255));    // WHITE
    restartText.setFillColor(Color(255, 255, 255));        // WHITE
    quitText.setFillColor(Color(255, 255, 255));        // WHITE
    controlText.setFillColor(Color(255, 255, 255));        // WHITE
    WASDText.setFillColor(Color(255, 255, 255));        // WHITE
    ArrowText.setFillColor(Color(255, 255, 255));        // WHITE

    paused.setCharacterSize(50);
    continueText.setCharacterSize(50);
    restartText.setCharacterSize(50);
    quitText.setCharacterSize(50);
    controlText.setCharacterSize(50);
    WASDText.setCharacterSize(45);
    ArrowText.setCharacterSize(45);

    paused.setPosition(Vector2f(435, 250));
    continueText.setPosition(Vector2f(435, 350));
    restartText.setPosition(Vector2f(435, 400));
    quitText.setPosition(Vector2f(435, 450));
    controlText.setPosition(Vector2f(435, 550));
    WASDText.setPosition(Vector2f(435, 600));
    ArrowText.setPosition(Vector2f(435, 600));
    // END PAUSE STATE

    /******************** MAIN GAME LOOP ********************/
    while (window.isOpen()) {
        deltaTime = deltaClock.restart();
        Event checkEvent;

        while (window.pollEvent(checkEvent)) {
            // WINDOW INTERACTION
            if (checkEvent.type == Event::Closed) {        // CLOSE WINDOW
                window.close();
            }
            else if (checkEvent.type == Event::Resized) {  // RESIZE WINDOW
                ResizeView(window, view);
                if (gamestate == PLAYING) {
                    gamestate = PAUSED;
                    soundSwitcher(isEdible, gamestate, siren, scatter);
                }
            }
            else if (checkEvent.type == Event::LostFocus) {  // INACTIVE WINDOW
                if (gamestate == PLAYING) {
                    gamestate = PAUSED;
                    soundSwitcher(isEdible, gamestate, siren, scatter);
                }
            }
            

            // GAME INTERACTION
            if (gamestate == PLAYING) { // PAUSE WITH SPACEBAR
                if (checkEvent.type == Event::KeyPressed && checkEvent.key.code == Keyboard::Space) {
                    gamestate = PAUSED;
                    soundSwitcher(isEdible, gamestate, siren, scatter);
                }
            }
            else if (gamestate == PAUSED) { // PAUSE MENU
                if (checkEvent.type == Event::KeyPressed) { // KEY PRESS ACTIONS
                    // CURSOR MOVEMENT
                    if (checkEvent.key.code == Keyboard::Down) { // SELECT DOWN
                        if (selectPos.y + 50 == 515) { // SKIPS BLANK SPACE
                            selectPos.y = 565;
                        }
                        else if (selectPos.y + 50 <= 615) { // MOVES CURSOR
                            selectPos.y += 50;
                            select.setPosition(selectPos);
                        }
                    }
                    else if (checkEvent.key.code == Keyboard::Up) { // SELECT UP
                        if (selectPos.y - 50 == 565) { // SKIPS BLANK SPACE
                            selectPos.y = 515;
                        }
                        if (selectPos.y - 50 >= 365) {    // MOVES CURSOR
                            selectPos.y -= 50;
                            select.setPosition(selectPos);
                        }
                    }
                    else if (checkEvent.key.code == Keyboard::Right) {   // SELECT RIGHT
                        if (selectPos.y == 615) {
                            pacman.WASDkeys = false;
                        }
                    }
                    else if (checkEvent.key.code == Keyboard::Left) {   // SELECT LEFT
                        if (selectPos.y == 615) {
                            pacman.WASDkeys = true;
                        }
                    }
                }
                // PERFORM SELECTED ACTION
                if (selectPos.y == 465 && checkEvent.key.code == Keyboard::Return) {       // SELECT QUIT (MENU)
                    window.close();
                }
                else if (selectPos.y == 415 && checkEvent.key.code == Keyboard::Return) {   // SELECT RESTART (MENU)
                    resetGame(pacman, blinky, inky, pinky, clyde, maze, edibleTime, gamestate);
                    resetStats(lifeCount, pelletCount, score, lifeScore, level, looppitch, fruit, ss, playerScore, startClock);

                    theme.play();
                    eatfruit.play();
                    isEdible = false;
                    startInit = false;
                    gamestate = STARTING;
                }
                else if (selectPos.y == 365 && checkEvent.key.code == Keyboard::Return) {   // SELECT CONTINUE
                    gamestate = PLAYING;
                    eatfruit.play();
                    soundSwitcher(isEdible, gamestate, siren, scatter);
                }
            }
            else if (gamestate == WINNER || gamestate == LOSER) {   // PRESS SPACE TO RESTART
                if (checkEvent.type == Event::KeyPressed && checkEvent.key.code == Keyboard::Space) {
                    resetGame(pacman, blinky, inky, pinky, clyde, maze, edibleTime, gamestate);

                    if (gamestate == LOSER) {
                        resetStats(lifeCount, pelletCount, score, lifeScore, level, looppitch, fruit, ss, playerScore, startClock);
                        theme.play();
                    }

                    eatfruit.play();
                    isEdible = false;
                    startInit = false;
                    gamestate = STARTING;
                }
            }
        }    // END OF WINDOW EVENTS POLLING

        /*** CHARACTER ANIMATIONS ***/
        pacman.update(deltaTime);
        blinky.update(deltaTime);
        inky.update(deltaTime);
        pinky.update(deltaTime);
        clyde.update(deltaTime);
        // END CHARACTER ANIMATIONS

        if (gamestate == STARTING) {
            if (!startInit) {
                startClock.restart().asSeconds();
                startInit = true;
            }

            startTime = startClock.getElapsedTime();
            if (deathTime.asSeconds() > 0.0 && deathTime.asSeconds() < 4.2) {
                pacman.switchState(REVIVED);
                managePlayerState(pacman);
            }
            if (startTime.asSeconds() >= 4.2) {
                gamestate = PLAYING;
                pacman.switchState(ALIVE);
                managePlayerState(pacman);
                soundSwitcher(isEdible, gamestate, siren, scatter);
                showFruit = false;
            }
        }
        else if (gamestate == DYING) {
            if (startInit) {
                startClock.restart().asSeconds();
            }
            deathTime = deathClock.getElapsedTime();
            
            if (deathTime.asSeconds() >= 1.475) {
                lifeCount--;    // DEDUCT LIFE AFTER DEATH ANIMATION
                lifeCount < 0 ? gamestate = LOSER : gamestate = STARTING;
                resetGame(pacman, blinky, inky, pinky, clyde, maze, edibleTime, gamestate);
            }
        }
        else if (gamestate == PLAYING) {       // MAIN GAME LOOP
            if (pelletCount >= 244) {
                gamestate = WINNER;
                setDifficulty(blinky, inky, pinky, clyde, edibleLimit);
                soundSwitcher(isEdible, gamestate, siren, scatter);
                levelUp(pelletCount, level, looppitch, siren, fruit, isEaten);
            }
            /*** CHARACTER MOVEMENT ***/
            pacman.nextDirection();
            pacman.setDirectionOpposite();
            for (int i = 0; i < 65; i++) {
                findNode(blinky, maze.node[i]);
                findNode(inky, maze.node[i]);
                findNode(pinky, maze.node[i]);
                findNode(clyde, maze.node[i]);
                findNode(pacman, maze.node[i]);
            }
            for (int i = 0; i < 7; i++) {
                findNode(blinky, maze.ghostNode[i]);
                findNode(inky, maze.ghostNode[i]);
                findNode(pinky, maze.ghostNode[i]);
                findNode(clyde, maze.ghostNode[i]);
            }
            blinky.blinkyAI(deltaTime, pacman);
            inky.inkyAI(deltaTime, pacman);
            pinky.pinkyAI(deltaTime, pacman);
            clyde.clydeAI(deltaTime, pacman);

            /*** COLLISIONS ***/
            if (blinky.isEdible) {
                edibleTime = edibleClock.getElapsedTime().asSeconds();
                
                manageGhostState(blinky, edibleTime, edibleLimit);
                manageGhostState(inky, edibleTime, edibleLimit);
                manageGhostState(pinky, edibleTime, edibleLimit);
                manageGhostState(clyde, edibleTime, edibleLimit);

                // EDIBLE GHOST SCORES
                ghostMultiplier(pacman, blinky, eatghost, ghostCount, score);
                ghostMultiplier(pacman, inky, eatghost, ghostCount, score);
                ghostMultiplier(pacman, pinky, eatghost, ghostCount, score);
                ghostMultiplier(pacman, clyde, eatghost, ghostCount, score);
                updatePoints(ss, score, playerScore);

                if (edibleTime >= edibleLimit) {
                    isEdible = false;
                    blinky.isEdible = inky.isEdible = pinky.isEdible = clyde.isEdible = false;
                    soundSwitcher(isEdible, gamestate, siren, scatter);
                    ghostCount = 0;
                }
            }
            else {
                if (checkCollision(pacman, blinky) || checkCollision(pacman, inky) || checkCollision(pacman, pinky) || checkCollision(pacman, clyde)) {
                    gamestate = DYING;
                    pacman.switchState(DEAD);
                    managePlayerState(pacman);
                    death.play();
                    soundSwitcher(isEdible, gamestate, siren, scatter);
                    deathClock.restart().asSeconds();
                }
            }

            /*** PELLET LOGIC ***/
            for (int i = 0; i < 240; i++) {   // SMALL PELLETS
                if (checkCollision(pacman, maze.pellet[i])) {
                    score = score + 10;
                    updatePoints(ss, score, playerScore);
                    oneUp(score, lifeScore, lifeCount, life);

                    pelletCount % 2 == 0 ? chomp1.play() : chomp2.play();

                    pelletCount++;
                    if (pelletCount % 50 == 0 && pelletCount != 0) {
                        looppitch += 0.025;        // INCREASE PITCH EVERY 50 PELLETS
                        siren.setPitch(looppitch);
                    }
                    maze.pellet[i].setPosition(Vector2f(0, WinY * 10));
                }
            }
            for (int i = 0; i < 4; i++) {        // POWER PELLETS
                if (checkCollision(pacman, maze.powerPellet[i])) {
                    score = score + 50;
                    updatePoints(ss, score, playerScore);
                    oneUp(score, lifeScore, lifeCount, life);
                    pelletCount++;
                    
                    blinky.isEdible = inky.isEdible = pinky.isEdible = clyde.isEdible = true;
                    isEdible = true;
                    edibleClock.restart();
                    
                    pelletCount % 2 == 0 ? chomp1.play() : chomp2.play();
                    soundSwitcher(isEdible, gamestate, siren, scatter);
                    
                    if (pelletCount % 50 == 0 && pelletCount != 0) {
                        looppitch += 0.025;        // INCREASE PITCH EVERY 50 PELLETS
                        siren.setPitch(looppitch);
                    }

                    maze.powerPellet[i].setPosition(Vector2f(0, WinY * 10));
                }
            }    // END PELLET LOGIC

            /*** FRUIT LOGIC ***/
            startTime = startClock.getElapsedTime();
            if (startTime.asSeconds() > 35.0 && startTime.asSeconds() < 35.1) {
                showFruit = true;
                fruitClock.restart().asSeconds();
            }
            if (showFruit) {
                fruitTime = fruitClock.getElapsedTime().asSeconds();
                if (fruitTime >= 0.0 && fruitTime <= 10.0) {
                    if (checkCollision(pacman, fruit)) {
                        switch (level) {
                            case 1: score = score + 100;
                                break;
                            case 2: score = score + 300;
                                break;
                            case 3: score = score + 500;
                                break;
                            case 4: score = score + 700;
                                break;
                            case 5: score = score + 1000;
                                break;
                            default: score = score + 2000;
                                break;
                        }

                        fruit.setPosition(pinkyPos);
                        updatePoints(ss, score, playerScore);
                        eatfruit.play();
                        isEaten = true;
                    }
                }
                if (fruitTime >= 9.99 && fruitTime <= 10.0)
                    showFruit = false;
            }
            // END FRUIT LOGIC

            // Teleports players to opposite x-coordinate when off screen
            maze.teleportPlayer(pacman);
            maze.teleportPlayer(blinky);
            maze.teleportPlayer(pinky);
            maze.teleportPlayer(inky);
            maze.teleportPlayer(clyde);
            
        }    // MAIN GAME LOOP END
        
        // MOVE AFTER TOUCHING NODE
        pacman.movePlayer(deltaTime, gamestate);
        blinky.moveGhost(deltaTime, gamestate);
        inky.moveGhost(deltaTime, gamestate);
        pinky.moveGhost(deltaTime, gamestate);
        clyde.moveGhost(deltaTime, gamestate);
        // END CHARACTER MOVEMENT

        window.setView(view);
        window.clear();

        /*** DRAWING ***/
        // ALWAYS DRAWING
        window.draw(background);        // MAZE BACKGROUND
        for (int i = 0; i < 240; i++) {   // SMALL PELLETS
            if (maze.pellet[i].getPosition().y <= WinY) {
                window.draw(maze.pellet[i]);
            }
        }

        for (int i = 0; i < 4; i++) {       // POWER PELLETS
            if (maze.powerPellet[i].getPosition().y <= WinY) {
                maze.powerPellet[i].update(deltaTime);
                window.draw(maze.powerPellet[i]);
            }
        }

        window.draw(playerScore);
        window.draw(scoreText);

        for (int i = 0; i < lifeCount; i++) {
            window.draw(lives[i]);
        }

        int fruitlimit = level < 7 ? level : 6;
        for (int i = 0; i < fruitlimit; i++) {
            window.draw(fruitSprite[i]);
        }

        // CONDITIONAL DRAWING
        if (gamestate == PLAYING) {
            fruit.update(deltaTime);
            if (!isEaten) {
                if (showFruit) {
                    window.draw(fruit);
                }
            }
        }

        if (gamestate != LOSER) {
            window.draw(pacman);
        }
        if (gamestate != DYING) {
            window.draw(blinky);
            window.draw(inky);
            window.draw(pinky);
            window.draw(clyde);
        }

        if (gamestate == LOSER) {                   // GAME OVER (LOSING)
            window.draw(gameover);
            window.draw(playAgain);
        }
        else if (gamestate == WINNER) {           // GAME OVER (WINNING)
            window.draw(youwin);
            window.draw(continueLevel);
        }
        else if (gamestate == STARTING) {
            window.draw(ready);
        }
        else if (gamestate == PAUSED) {   // PAUSE MENU
            window.draw(pauseOpacity);
            window.draw(pauseMenu);
            window.draw(paused);
            window.draw(continueText);
            window.draw(restartText);
            window.draw(quitText);
            window.draw(controlText);
            window.draw(select);

            if (pacman.WASDkeys) {
                window.draw(WASDText);
            }
            else {
                window.draw(ArrowText);
            }
        }
        // END DRAWING

        window.display();
    }
    return 0;
}

