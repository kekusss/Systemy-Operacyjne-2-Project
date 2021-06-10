#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <ncurses.h>
#include <unistd.h>
#include <mutex>
#include <condition_variable>
#include "Car.h"

bool running = true;
std::vector<Car> cars;
std::vector<std::thread> carsThreads;

std::mutex mutexes[1000];
std::condition_variable cvs[1000];
bool isStanding[1000];

/**
 * Zadanie 3:
 * Jeśli pojazd dogoni inny to będzie stawał i stał określny czas a następnie będzie ruszał.
 * Uśpiony pojazd blokuje drogę, za nim ustawiają się inne pojazdy, kiedy pierwszy ruszy to inne pojazdy ruszają z opóźnieniem,
 * każdy z kolejki będzie wypuszczany co jakiś czas
*/

void myClear(){
    int xMax, yMax = 0;
    getmaxyx(stdscr, yMax, xMax);

    for(int i=0; i<xMax-10; i++){
        mvprintw(0 , i, " ");
        mvprintw(yMax-10 , i, " ");
    }

    for(int i=0; i<yMax-11; i++){
        mvprintw(i+1 , 0, " ");
        mvprintw(i+1 , xMax-10, " ");
    }

    for(int i=0; i<3; i++){
        mvprintw(i , xMax-10, " ");
        mvprintw(yMax-10-i, xMax-10, " ");
        mvprintw(yMax-10 , xMax-12+i, " ");
    }
}

void drawTrack(){
    int xMax, yMax = 0;
    getmaxyx(stdscr, yMax, xMax);

    for(int i=1; i<xMax-11; i++){
        mvprintw(1 , i, "_");
        mvprintw(yMax-11 , i, "_");
        mvprintw(yMax-9 , i, "_");
    }

    for(int i=1; i<yMax-11; i++){
        mvprintw(i+1 , 1, "|");
        mvprintw(i+1 , xMax-11, "|");
        mvprintw(i+1 , xMax-8, "|");
    }

    for(int i=0; i<3; i++){
        mvprintw(i , xMax-8, "|");
        mvprintw(yMax-9-i, xMax-8, "|");
        mvprintw(yMax-9 , xMax-11+i, "_");
    }

}

// "Monitor" function
void refreshScreen()
{
    while(running == true)
    {
        myClear();

        for(int i = 0; i < (int)cars.size(); i++)
        {
            if(cars[i].isDriving()){
                mvprintw(cars[i].getyPosition(), cars[i].getxPosition(), cars[i].getSymbol() );
            }
        }
        usleep(47000);
        refresh();

        // Refresh every 0.05 s

        // Check if any button has been pressed
        int ch = getch();
        if (ch != ERR) {
            ungetch(ch);
            for(int i = 0; i < (int)cars.size(); i++){
                cars[i].kill();
            }
            running = false;
        }
    }
}

int main(int argc, char *argv[])
{
    int xMax, yMax, ridingCars = 0;

    srand(time(0));

    // Initialize ncurses
    initscr();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    scrollok(stdscr, TRUE);
    curs_set(0);

    // Get console dimensions
    getmaxyx(stdscr, yMax, xMax);

    drawTrack();

    // Initialize scene
    Car::initScene(abs(xMax-10), abs(yMax-10));

    char ids[60] = {'a','\0', 'b','\0','c','\0','d','\0','e','\0','f','\0','g','\0','h','\0','i','\0','j','\0','k','\0','l','\0','m','\0','n','\0','o','\0','p','\0','q','\0','r','\0','s','\0','t','\0','u','\0','v','\0','w','\0','x','\0','y','\0','z'};
    
    // Initialize all cars
    int max = rand() % 10 + 5;

    for(int i=0; i<1000; i++){
        isStanding[i] = false;
    }
    
    for(int i = 0; i < max; i++)
    {
        Car car(0, 0, &ids[i*2], (rand() % 60) + 40, mutexes, cvs, isStanding);
        cars.push_back(car);
    }

    // Start monitor
    std::thread monitor(refreshScreen);

    int randCar = 0;
    // Run cars
    while(running == true && ridingCars < max)
    {
        // Determine car...
        while(cars.at(randCar).isDriving())
        {
            // ...which still isn't riding
            randCar++;
        }

        carsThreads.push_back(cars.at(randCar).driveThread());
        ridingCars++;

        // Random time until start of next car
        unsigned randTime = rand() % 20 + 1;
        usleep(100000 * randTime);
    }
    
    // Wait for all cars
    for(int i = 0; i < (int)carsThreads.size(); i++)
    {
        carsThreads.at(i).join();
    }

    sleep(1);
    running = false;
    
    // Stop monitor
    monitor.join();

    // Close ncurses
    endwin();

    return 0;
}
