#include "Car.h"

bool Car::initialized = false;
int Car::xMax;
int Car::yMax;
int Car::maxLaps = 3;
int Car::xStart;
int Car::yStart;

Car::Car(int xPosition, int yPosition, const char* idCharacter, int speed, std::mutex (&mutexesArg)[1000], std::condition_variable (&cvsArg)[1000], bool (&isStandingArg)[1000])
    : mutexes(mutexesArg) , cvs(cvsArg), isStanding(isStandingArg)
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

void Car::move(){
    // both vertical sections
    if(yPosition > 0 && yPosition < yMax){
        if(xPosition == 0){
            yPosition -= 1;
        }
        else{
            yPosition += 1;
        }
    }
    // first horizontal section
    else if(yPosition <= 0){
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
        if(xPosition <= 0){
            xPosition = 0;
            yPosition = yMax - 1;
        }
        else{
            xPosition -= 1;
        }
    }
}
int Car::getIndex(){
    int index = 0;
    // both vertical sections
    if(yPosition > 0 && yPosition < yMax){
        if(xPosition == 0){
            index = 2*xMax + 2*yMax - yPosition;
        }
        else{
            index = xMax+yPosition;
        }
    }
    // first horizontal section
    else if(yPosition <= 0){
        index = xPosition;
    }
    // second horizontal section
    else{
        index = xMax + yMax +  xMax - xPosition;
    }
    return index;
}

float Car::getSpeedOfTrack(){
    if(yPosition > 0 && yPosition < yMax){
        return 1.8;
    }
    else{
        return 0.6;
    }
}

void Car::drive()
{
    if(initialized)
    {
        driving = true;

        while(lap < maxLaps && driving == true)
        {
            // get position index
            int index = getIndex();
            // try to lock next position
            std::unique_lock<std::mutex> lock(mutexes[index+1], std::try_to_lock);


            if(lock.owns_lock()){
                move();
                // sytuacja gdy ktos czeka a?? go wyprzedzimy
                if(isStanding[index+1]){
                    lock.unlock();
                    cvs[index+1].notify_one();
                }
                else{
                    // my stoimy i czekamy az ktos nas wyprzedzi
                    isStanding[index+1] = true;
                    auto now = std::chrono::system_clock::now();
                    // blocks the current thread until the condition variable wakes up or on the timeout side
                    if (cvs[index+1].wait_until(lock, now + (std::chrono::microseconds((int)((3000000/speed)*getSpeedOfTrack())))) != std::cv_status::timeout){
                        // sleep
                        usleep(2000000);
                    }
                    lock.unlock();
                    // notify
                    isStanding[index+1] = false;
                }
            }
            // ktos zostal zatrzymany
            else{
                // my tez jestesmy zatrzymani
                isStanding[index] = true;
                std::unique_lock<std::mutex> lock2(mutexes[index]);
                
                // czekamy az samochod przed nami ruszy, problem jednego piszacego, wielu czytajacych
                while(isStanding[index+1] == true){
                }
                usleep(1000000);
                isStanding[index] = false;
                lock2.unlock();
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
