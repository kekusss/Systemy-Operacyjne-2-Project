#include "Car.h"

bool Car::initialized = false;
int Car::xMax;
int Car::yMax;
int Car::maxLaps = 3;
int Car::xStart;
int Car::yStart;

Car::Car(int xPosition, int yPosition, const char* idCharacter, int speed)
{
    //ctor
    this->xPosition = Car::xStart = xPosition;
    this->yPosition = Car::yStart = yPosition;
    this->idCharacter = idCharacter;
    this->speed = speed;
    this->driving = false;
    this->lap = 0;
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
                usleep((int) (3000000/speed)*1.8);

                if(xPosition == 0) yPosition -= 1;
                else yPosition +=1;
            }
            // first horizontal section
            else if(yPosition <= 0){
                usleep((int) (3000000/speed)*0.6);

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
                usleep((int) (3000000/speed)*0.6);
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
