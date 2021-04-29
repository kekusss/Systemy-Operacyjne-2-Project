#include "Car.h"

bool Car::initialized = false;
int Car::xMax;
int Car::yMax;
int Car::maxLaps = 3;
int Car::xStart;
int Car::yStart;

Car::Car(int xPosition, int yPosition, const char* idCharacter, int speed, std::mutex (&mutexesArg)[1000], std::condition_variable (&cvsArg)[1000])
    : mutexes(mutexesArg) , cvs(cvsArg)
{
    //ctor
    this->xPosition = Car::xStart = xPosition;
    this->yPosition = Car::yStart = yPosition;
    this->idCharacter = idCharacter;
    this->speed = speed;
    this->driving = false;
    this->lap = 0;
    this->locked = false;
}

Car::~Car()
{
    //dtor
}

void Car::initScene(int xRes, int yRes)
{
    xMax = xRes;
    yMax = yRes;
    initialized = true;
}

int Car::getSpeed(){
    return speed;
}

int Car::getxPosition()
{
    return xPosition;
}

int Car::getyPosition()
{
    return yPosition;
}

int Car::getLap()
{
    return lap;
}

bool Car::isDriving()
{
    return driving;
}

const char* Car::getSymbol(){
    return idCharacter;
}

void Car::kill(){
    this->driving = false;
}

void Car::drive()
{
    if(initialized)
    {
        driving = true;

        while(lap < maxLaps && driving == true)
        {
            // both vertical sections
            if(yPosition > 0 && yPosition < yMax){

                if(xPosition == 0){
                    int index = 2*xMax + yMax + yPosition;
                    
                    // notifies cars in a given position that he is trying to seize it
                    cvs[index].notify_one();

                    // blokuje mutex
                    std::unique_lock<std::mutex> lock(mutexes[index]);

                    // blocks the current thread until the condition variable wakes up or on the timeout side
                    auto now = std::chrono::system_clock::now();
                    if (cvs[index].wait_until(lock, now + (std::chrono::microseconds((int)((3000000/speed)*1.8)))) != std::cv_status::timeout)
                    {
                        lock.unlock();
                        usleep(3000000);
                    }

                    yPosition -= 1;
                }
                else{
                    int index = xMax+yPosition;
                    
                    // notifies cars in a given position that he is trying to seize it
                    cvs[index].notify_one();

                    // blokuje mutex
                    std::unique_lock<std::mutex> lock(mutexes[index]);

                    // blocks the current thread until the condition variable wakes up or on the timeout side
                    auto now = std::chrono::system_clock::now();
                    if (cvs[index].wait_until(lock, now + (std::chrono::microseconds((int)((3000000/speed)*1.8)))) != std::cv_status::timeout)
                    {
                        lock.unlock();
                        usleep(3000000);
                    }
            
                    yPosition +=1;
                }
            }
            // first horizontal section
            else if(yPosition <= 0){
                int index = xPosition;
                
                // notifies cars in a given position that he is trying to seize it
                cvs[index].notify_one();

                // blokuje mutex
                std::unique_lock<std::mutex> lock(mutexes[index]);

                // blocks the current thread until the condition variable wakes up or on the timeout side
                auto now = std::chrono::system_clock::now();
                if (cvs[index].wait_until(lock, now + (std::chrono::microseconds((int)((3000000/speed)*0.6)))) != std::cv_status::timeout)
                {
                    lock.unlock();
                    usleep(3000000);
                }

                if(xPosition >= xMax){
                    yPosition = 1;
                    xPosition = xMax;
                }
                else{
                    xPosition += 1;
                }
            }
            // second horizontal section
            else{
                int index = xMax + yMax + xPosition;
                
                // notifies cars in a given position that he is trying to seize it
                cvs[index].notify_one();

                // try to lock mutex
                std::unique_lock<std::mutex> lock(mutexes[index], std::try_to_lock);
                
                // blocks the current thread until the condition variable wakes up or on the timeout side
                auto now = std::chrono::system_clock::now();
                if (cvs[index].wait_until(lock, now + (std::chrono::microseconds((int)((3000000/speed)*0.6)))) != std::cv_status::timeout)
                {
                    lock.unlock();
                    usleep(3000000);
                }

                if(xPosition <= 0){
                    xPosition = 0;
                    yPosition = yMax - 1;
                }
                else{
                    xPosition -= 1;
                }
            }

            if(xPosition == xStart && yPosition == yStart)
            {
                 lap++;
            }
        }
        driving = false;
        initialized = false;
    }
}

std::thread Car::driveThread()
{
    return std::thread(&Car::drive, this);
}
