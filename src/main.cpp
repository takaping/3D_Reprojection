/* 
 * The main routine of this application.
 * 
 * File:   main.cpp
 * Author: munehiro
 *
 * Created on February 28, 2014, 5:56 PM
 */

#include <iostream>
#include <gtkmm-2.4/gtkmm.h>
#include <gtkglextmm-1.2/gtkglmm.h>
#include "MainWindow.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    Gtk::Main kit(argc, argv);
    Gtk::GL::init(argc, argv);
    Glib::RefPtr<Gtk::Builder> builder;
    try {
        builder = Gtk::Builder::create_from_file("MainWindow.glade");
    } catch (const Glib::FileError& ex) {
        cerr << ex.what() << endl;
    }
    MainWindow* mainWindow = nullptr;
    builder->get_widget_derived("main_window", mainWindow);
    if (mainWindow) {
        kit.run(*mainWindow);
    }

    return 0;
}

