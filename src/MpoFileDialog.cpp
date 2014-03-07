/* 
 * MpoFileDialog Class
 *  - The dialog to choose the MPO file
 * 
 * File:   MpoFileDialog.cpp
 * Author: munehiro
 * 
 * Created on January 31, 2014, 10:01 PM
 */

#include <gtkmm-2.4/gtkmm/filefilter.h>
#include "MpoFileDialog.h"

/**
 * Constructor and Destructor
 * @param parent
 * @param title
 * @param action to open or save
 */
MpoFileDialog::MpoFileDialog(Gtk::Window& parent, const Glib::ustring& title, Gtk::FileChooserAction action)
: Gtk::FileChooserDialog(parent, title, action) {
    // configure the cancel, open/save buttons
    add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    if (action == Gtk::FILE_CHOOSER_ACTION_OPEN) {
        add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);
    } else {
        add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_OK);
    }
    // set the file filter
    Gtk::FileFilter image_filter;
    image_filter.set_name("MPO Files");
    image_filter.add_pattern("*.mpo");
    image_filter.add_pattern("*.MPO");
    add_filter(image_filter);
    Gtk::FileFilter no_filter;
    no_filter.set_name("All");
    no_filter.add_pattern("*");
    add_filter(no_filter);
}

MpoFileDialog::~MpoFileDialog() {
}

