#include <iostream>
#include <random>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */


template <typename T>
T MessageQueue<T>::receive()
{
    std::unique_lock<std::mutex> uLock(_mutex);
    _condition.wait(uLock, [this] { return !_queue.empty(); }); 
    T message = std::move(_queue.back());
    _queue.pop_back();
    return message;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _queue.push_back(std::move(msg));
    _condition.notify_one(); 
}


/* Implementation of class "TrafficLight" */


TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    while(true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        if(TrafficLightPhase::green == _messageQueue.receive()) return;
    }
    
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::setCurrentPhase(TrafficLightPhase phase)
{
    _currentPhase = phase;
}

void TrafficLight::simulate()
{
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    while(true)
    {
        int r = 4 + (rand() % 3);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000*r));
        (getCurrentPhase() == TrafficLightPhase::red)?
                     setCurrentPhase(TrafficLightPhase::green):
                     setCurrentPhase(TrafficLightPhase::red);
        _messageQueue.send(std::move(getCurrentPhase()));
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}
