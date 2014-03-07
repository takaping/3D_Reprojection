/* 
 * MainWindow Class
 *  - The main window of this application.
 *  - There are 2 views, the one is the polygon view
 *    and the other is the image view.
 *  - There are 4 menus that open the MPO file to construct the 3D polygon,
 *    quit this application, calibrate the stereo camera
 *    and edit the calibration rig properties.
 * 
 * File:   MainWindow.h
 * Author: munehiro
 *
 * Created on February 28, 2014, 6:00 PM
 */

#ifndef MAINWINDOW_H
#define	MAINWINDOW_H

#include <memory>
#include <gtkmm-2.4/gtkmm.h>
#include "RigDialog.h"

using namespace std;

class GraphicsModel;
class ImageView;
class SceneView;

class MainWindow : public Gtk::Window {
public:
    MainWindow(BaseObjectType* object, const Glib::RefPtr<Gtk::Builder>& builder);
    virtual ~MainWindow();
private:
    void open();
    void calibrate();
    void editRigProperty();
    shared_ptr<GraphicsModel> model;    // model of the model/view architecture
    shared_ptr<ImageView> iView;        // image view of the model/view architecture
    shared_ptr<SceneView> sView;        // scene view of the model/view architecture
    RigDialog::Pattern ptn; // type of the calibration rig pattern
    int rows, cols;         // number of rows, columns of the calibration rig pattern
    double dist;            // distance between corners or centers of the calibration rig pattern

};

#endif	/* MAINWINDOW_H */

