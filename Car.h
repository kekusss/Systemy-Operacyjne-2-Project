#ifndef BRICK_H
#define BRICK_H

#include <iostream>
#include <thread>
#include <unistd.h>

class Car
{
    public:
        Car(int xPosition, int yPosition, const char* idCharacter, int speed);
        ~Car();
        static void initScene(int xRes, int yRes);
        static bool initialized;
        int getxPosition();
        int getyPosition();
        int getLap();
        int getSpeed();
        const char * getSymbol();
        bool isDriving();
        void kill();
        std::thread driveThread();
    protected:
    private:
        static int xMax;
        static int yMax;
        static int maxLaps;
        static int xStart;
        static int yStart;
        const char* idCharacter;
        int xPosition;
        int yPosition;
        int speed;
        int lap;
        bool driving;
        void drive();
};

#endif
