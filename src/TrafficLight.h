#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"

// forward declarations to avoid include cycle
class Vehicle;

template <class T>
class MessageQueue
{
public:
    void send(T &&message);
    T receive();

private:
    std::condition_variable _condition;
    std::mutex _mutex;
    std::deque<T> _queue;
};

enum TrafficLightPhase
{
    noLight,
    red,
    green
};

class TrafficLight : public TrafficObject
{
public:
    // constructor / desctructor
    TrafficLight();

    // getters / setters
    TrafficLightPhase getCurrentPhase();
    void setCurrentPhase(TrafficLightPhase phase);
    bool isGreen();
    
    // typical behaviour methods
    void waitForGreen();
    void simulate();
    

private:
    // typical behaviour methods
    TrafficLightPhase _currentPhase;
    void cycleThroughPhases();

    MessageQueue<TrafficLightPhase> _messageQueue;
    std::condition_variable _condition;
    std::mutex _mutex;
};

#endif