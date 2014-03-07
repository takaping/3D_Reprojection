/* 
 * Observer class
 *  - The observer of Observer pattern
 * 
 * File:   Observer.h
 * Author: munehiro
 *
 * Created on February 1, 2014, 12:54 AM
 */

#ifndef OBSERVER_H
#define	OBSERVER_H

class Subject;

class Observer {
public:
    Observer();
    virtual ~Observer();
    virtual void update(const Subject* subject) = 0;
private:

};

#endif	/* OBSERVER_H */

