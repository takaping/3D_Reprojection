/* 
 * MainWindow Class
 *  - The main window of this application.
 *  - There are 2 views, the one is the polygon view
 *    and the other is the image view.
 *  - There are 4 menus that open the MPO file to construct the 3D polygon,
 *    quit this application, calibrate the stereo camera
 *    and set the calibration rig properties.
 * 
 * File:   MainWindow.cpp
 * Author: munehiro
 * 
 * Created on February 28, 2014, 6:00 PM
 */

#include <vector>
#include "MainWindow.h"
#include "MpoFileDialog.h"
#include "GraphicsModel.h"
#include "ImageView.h"
#include "SceneView.h"

/**
 * Constructor and Destructor
 * @param object
 * @param builder
 */
MainWindow::MainWindow(BaseObjectType* object, const Glib::RefPtr<Gtk::Builder>& builder)
: Gtk::Window(object)
, ptn(RigDialog::Pattern::Chessboard), rows(6), cols(9), dist(27.0) {
    // set the model/view architecture
    model.reset(new GraphicsModel);
    iView.reset(new ImageView);
    sView.reset(new SceneView);
    model->attach(iView);
    model->attach(sView);
    // relate between the widget and the variable
    Gtk::ImageMenuItem *openMenu, *quitMenu;
    Gtk::MenuItem *calibMenu, *rigMenu;
    Gtk::Viewport *imageView, *sceneView;
    builder->get_widget("open_menuitem", openMenu);
    builder->get_widget("quit_menuitem", quitMenu);
    builder->get_widget("calib_menuitem", calibMenu);
    builder->get_widget("rig_menuitem", rigMenu);
    builder->get_widget("image_view", imageView);
    builder->get_widget("scene_view", sceneView);
    // set the signal and the slot
    openMenu->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::open));
    quitMenu->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::hide));
    calibMenu->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::calibrate));
    rigMenu->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::editRigProperty));
    // add the image and the scene view to the viewport
    imageView->add(*iView);
    sceneView->add(*sView);
    
    show_all_children();
}

MainWindow::~MainWindow() {
}

/**
 * Open the MPO file
 */
void MainWindow::open() {
    MpoFileDialog dlg(*this, "Choose a MPO");
    if (dlg.run() != Gtk::RESPONSE_OK) {
        return;
    }
    string fn = dlg.get_filename();
    try {
        model->open(fn);
    } catch (const string& msg) {
        Gtk::MessageDialog dlg(msg, false, Gtk::MESSAGE_WARNING);
        dlg.run();
    }
}

/**
 * Calibrate the stereo camera
 */
void MainWindow::calibrate() {
    MpoFileDialog dlg(*this, "Choose a MPOs");
    dlg.set_select_multiple(true);
    if (dlg.run() != Gtk::RESPONSE_OK) {
        return;
    }
    vector<string> fns = dlg.get_filenames();
    try {
        model->calibrate(fns, ptn, rows, cols, dist);
    } catch (const string& msg) {
        Gtk::MessageDialog dlg(msg, false, Gtk::MESSAGE_WARNING);
        
        dlg.run();
    }
}

/**
 * Edit the calibration rig properties
 */
void MainWindow::editRigProperty() {
    Glib::RefPtr<Gtk::Builder> builder;
    try {
        builder = Gtk::Builder::create_from_file("RigDialog.glade");
    } catch (const Glib::FileError& ex) {
        Gtk::MessageDialog dlg(ex.what(), false, Gtk::MESSAGE_WARNING);
        dlg.run();
    }
    RigDialog* rigDialog = nullptr;
    builder->get_widget_derived("rig_dialog", rigDialog);
    rigDialog->setPattern(ptn);
    rigDialog->setRows(rows);
    rigDialog->setColumns(cols);
    rigDialog->setDistance(dist);
    if (rigDialog->run() != Gtk::RESPONSE_OK) {
        return;
    }
    try {
        ptn = rigDialog->pattern();
        rows = rigDialog->rows();
        cols = rigDialog->columns();
        dist = rigDialog->distance();
    } catch (const string& msg) {
        Gtk::MessageDialog dlg(msg, false, Gtk::MESSAGE_WARNING);
        dlg.run();
    }
}

