#include <iostream>
#include <random>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */


template <typename T>
T MessageQueue<T>::receive()
{
    std::unique_lock<std::mutex> uLock(_mutex);
    _cond.wait(uLock, [this]{
        return !_queue.empty();
        }
    );
    T tlp = std::move(_queue.back());
    _queue.pop_back();
    return tlp;
    // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait() 
    // to wait for and receive new messages and pull them from the queue using move semantics. 
    // The received object should then be returned by the receive function. 
}

template <typename T>
void MessageQueue<T>::send(T &&current_phase)
{   
    std::lock_guard<std::mutex> uLock(_mutex);
    _queue.emplace_back(std::move(current_phase));
    _cond.notify_one();
    // FP.4a : The method send should use the mechanisms std::lock_guard<std::mutex> 
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.
}


/* Implementation of class "TrafficLight" */


TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop 
    // runs and repeatedly calls the receive function on the message queue. 
    // Once it receives TrafficLightPhase::green, the method returns.
    while(true){
        auto current_phase = _messageQueue.receive();
        if(current_phase == TrafficLightPhase::green){
            return ;
        }

    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    // FP.2b : Finally, the private method „cycleThroughPhases“ should be started in a thread when the public method „simulate“ is called. To do this, use the thread queue in the base class. 
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases,this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{   
    std::chrono::time_point<std::chrono::system_clock> lastUpdate;
    // init stop watch
    lastUpdate = std::chrono::system_clock::now();
    std::random_device ran_dev;
    std::mt19937 gen(ran_dev());
    std::uniform_int_distribution<> distr(4000, 6000);
    auto cycleDuration = std::chrono::milliseconds(distr(gen));
    while(true){
        // compute time difference to stop watch
        auto timeSinceLastUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - lastUpdate);
        //double cycleDuration = 4000; // duration of a single simulation cycle in ms
        
        
        if (timeSinceLastUpdate>=cycleDuration)
        {   
           
            if (_currentPhase == TrafficLightPhase::red)
            {
                _currentPhase = TrafficLightPhase::green;
                 //std::cout<< "green"<<std::endl;
            }
            else
            {
                _currentPhase = TrafficLightPhase::red;
                //std::cout<< "red"<<std::endl;
            }
            _messageQueue.send(std::move(_currentPhase));
            lastUpdate = std::chrono::system_clock::now();
            auto cycleDuration = std::chrono::milliseconds(distr(gen));
        }
        // sleep at every iteration to reduce CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    // FP.2a : Implement the function with an infinite loop that measures the time between two loop cycles 
    // and toggles the current phase of the traffic light between red and green and sends an update method 
    // to the message queue using move semantics. The cycle duration should be a random value between 4 and 6 seconds. 
    // Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles. 
}

