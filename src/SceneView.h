/* 
 * SceneVew Class
 *  - The view of the model/view architecture is implemented
 *    to render the 3D polygon.
 *  - Gtk::GL::DrawingArea of Gtkglextmm Library and Observer class are inherited.
 *  - The 3D point cloud, the edges and surfaces of the 3D polygon are rendered.
 *  - The 3D polygon is rotated by the mouse drag.
 *  - The 3D polygon is zoomed in/out by the mouse wheel.
 * 
 * File:   SceneView.h
 * Author: munehiro
 *
 * Created on March 1, 2014, 7:35 PM
 */

#ifndef SCENEVIEW_H
#define	SCENEVIEW_H

#include <vector>
#include <gtkglextmm-1.2/gtkglmm.h>
#include "Observer.h"

typedef unsigned int uint;

using namespace std;

class Vertex;

class SceneView : public Gtk::GL::DrawingArea, public Observer {
public:
    SceneView();
    virtual ~SceneView();
    virtual void update(const Subject* subject);
protected:
    virtual void on_realize();
    virtual bool on_configure_event(GdkEventConfigure* event);
    virtual bool on_expose_event(GdkEventExpose* event);
    virtual bool on_button_press_event(GdkEventButton* event);
    virtual bool on_motion_notify_event(GdkEventMotion* event);
    virtual bool on_scroll_event(GdkEventScroll* event);
private:
    // the eye point, the near and the far clip of the scene view
    static const GLdouble EYEPOINT, NEARCLIP, FARCLIP;
    void renderPointCloud();
    void renderEdges();
    void renderSurface();
    // x and y position of the mouse, the quaternion for the polygon rotation
    float mx, my, currQ[4];
    double scl;             // scale of the polygon
    vector<Vertex> vtcs;                // vertices of the polygon
    vector<vector<uint>> faceVtxIdxs;   // vertex indexes of the polygon surfaces

};

#endif	/* SCENEVIEW_H */

