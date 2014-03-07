/* 
 * ImageView Class
 *  - The view of the model/view architecture is implemented
 *    to draw the image.
 *  - Gtk::DrawingArea of Gtkmm Library and Observer class are inherited.
 * 
 * File:   ImageView.cpp
 * Author: munehiro
 * 
 * Created on February 28, 2014, 7:38 PM
 */

#include "ImageView.h"
#include "GraphicsModel.h"
#include "Image.h"

/**
 * Constructor and Destructor
 */
ImageView::ImageView() : img(nullptr) {
}

ImageView::~ImageView() {
}

/**
 * Update the image view
 * @param subject
 */
void ImageView::update(const Subject* subject) {
    this->img.reset();
    Image* img = ((GraphicsModel*)subject)->image();
    if (img && !img->isEmpty()) {
        // get the image
        // the color order of OpenCV is BGR, then convert BGR to RGB
        this->img.reset(new Image(img->rgbImage()));
        queue_draw();
    }
}

/**
 * On realize
 */
void ImageView::on_realize() {
    Gtk::DrawingArea::on_realize();
    Gdk::Color col;
    col.set_rgb(65535, 65535, 65535);
    modify_bg(Gtk::STATE_NORMAL, col);
}

/**
 * On expose event
 * @param event
 * @return 
 */
bool ImageView::on_expose_event(GdkEventExpose* event) {
    if (!img) {
        return false;
    }
    // resize the image to the window size
    int ww = get_width();
    int wh = get_height();
    int iw = img->width();
    int ih = img->height();
    double wr = (double)ww / (double)iw;
    double hr = (double)wh / (double)ih;
    int w = 0;
    int h = 0;
    if (wr < hr) {
        w = ww;
        h = (int)(wr * (double)ih);
    } else {
        w = (int)(hr * (double)iw);
        h = wh;
    }
    Glib::RefPtr<Gdk::Pixbuf> pixbuf = Gdk::Pixbuf::create_from_data((const guint8*)img->data(),
            Gdk::COLORSPACE_RGB, false, 8, iw, ih, img->rowstride())->scale_simple(w, h, Gdk::INTERP_BILINEAR);
    Cairo::RefPtr<Cairo::Context> context = get_window()->create_cairo_context();
    Gdk::Cairo::set_source_pixbuf(context, pixbuf, 0, 0);
    context->paint();

    return true;
}

