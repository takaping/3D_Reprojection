/* 
 * ImageView Class
 *  - The view of the model/view architecture is implemented
 *    to draw the image.
 *  - Gtk::DrawingArea of Gtkmm Library and Observer class are inherited.
 * 
 * File:   ImageView.h
 * Author: munehiro
 *
 * Created on February 28, 2014, 7:38 PM
 */

#ifndef IMAGEVIEW_H
#define	IMAGEVIEW_H

#include <memory>
#include <gtkmm-2.4/gtkmm.h>
#include "Observer.h"

using namespace std;

class Image;

class ImageView : public Gtk::DrawingArea, public Observer {
public:
    ImageView();
    virtual ~ImageView();
    virtual void update(const Subject* subject);
protected:
    virtual void on_realize();
    virtual bool on_expose_event(GdkEventExpose* event);
private:
    shared_ptr<Image> img;  // image

};

#endif	/* IMAGEVIEW_H */

