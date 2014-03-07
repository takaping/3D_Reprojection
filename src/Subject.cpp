/* 
 * Subject class
 *  - The subject of Observer pattern.
 * 
 * File:   Subject.cpp
 * Author: munehiro
 * 
 * Created on February 1, 2014, 12:56 AM
 */

#include <algorithm>
#include "Subject.h"
#include "Observer.h"

/**
 * Constructor and Destructor
 */
Subject::Subject() {
}

Subject::~Subject() {
}

/**
 * Attach the observer to the observer list
 * @param observer
 */
void Subject::attach(const shared_ptr<Observer> observer) {
    if (std::find(observers.begin(), observers.end(), observer) == observers.end()) {
        observers.push_back(observer);
    }
}

/**
 * Detach the observer from the observer list
 * @param observer
 */
void Subject::detach(const shared_ptr<Observer> observer) {
    auto it = std::find(observers.begin(), observers.end(), observer);
    if (it != observers.end()) {
        observers.erase(it);
    }
}

/**
 * Update observers
 */
void Subject::notify() {
    for_each(observers.begin(), observers.end(), [this](shared_ptr<Observer> observer) {
        observer->update(this);
    });
}

