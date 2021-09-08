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
    _queue.clear();
    return message;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _queue.emplace_back(msg);
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
    std::random_device rd;
    std::mt19937 eng(rd());
    std::uniform_real_distribution<> dist(4.0,6.0);
    double cycle_duration = dist(eng);
    auto cycle_start_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> cycle_elapsed_time;
    while(true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        cycle_elapsed_time = std::chrono::high_resolution_clock::now() - cycle_start_time;
        if(cycle_elapsed_time.count() > cycle_duration)
        { 
             (getCurrentPhase() == TrafficLightPhase::red)?
                       setCurrentPhase(TrafficLightPhase::green):
                       setCurrentPhase(TrafficLightPhase::red);
             _messageQueue.send(std::move(getCurrentPhase()));
             cycle_start_time = std::chrono::high_resolution_clock::now();
             cycle_duration = dist(eng);
        }
    }
}
