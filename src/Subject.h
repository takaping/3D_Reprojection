/* 
 * Subject class
 *  - The subject of Observer pattern.
 * 
 * File:   Subject.h
 * Author: munehiro
 *
 * Created on February 1, 2014, 12:56 AM
 */

#ifndef SUBJECT_H
#define	SUBJECT_H

#include <memory>
#include <vector>

using namespace std;

class Observer;

class Subject {
public:
    Subject();
    virtual ~Subject();
    void attach(const shared_ptr<Observer> observer);
    void detach(const shared_ptr<Observer> observer);
    void notify();
private:
    vector<shared_ptr<Observer>> observers;     // observer list

};

#endif	/* SUBJECT_H */

