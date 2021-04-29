#include <iostream>
#include <thread>
#include <unistd.h>
#include <mutex>
#include <condition_variable>

class Car
{
    public:
        Car(int xPosition, int yPosition, const char* idCharacter, int speed, std::mutex (&mutexesArg)[1000], std::condition_variable (&cvsArg)[1000]);
        ~Car();
        static void initScene(int xRes, int yRes);
        static bool initialized;
        int getxPosition();
        int getyPosition();
        int getLap();
        int getSpeed();
        void lock();
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
        int locked;
        int xPosition;
        int yPosition;
        int speed;
        int lap;
        bool driving;
        void drive();
        std::mutex (&mutexes)[1000];
        std::condition_variable (&cvs)[1000];
};
