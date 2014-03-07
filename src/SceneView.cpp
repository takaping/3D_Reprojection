/* 
 * SceneVew Class
 *  - The view of the model/view architecture is implemented
 *    to render the 3D polygon.
 *  - Gtk::GL::DrawingArea of Gtkglextmm Library and Observer class are inherited.
 *  - The 3D point cloud, the edges and surfaces of the 3D polygon are rendered.
 *  - The 3D polygon is rotated by the mouse drag.
 *  - The 3D polygon is zoomed in/out by the mouse wheel.
 * 
 * File:   SceneView.cpp
 * Author: munehiro
 * 
 * Created on March 1, 2014, 7:35 PM
 */

#include "SceneView.h"
#include "GraphicsModel.h"
#include "Polygon.h"
#include "Vertex.h"
extern "C" {
    #include "trackball.h"
}

const GLdouble SceneView::EYEPOINT = 30.0;
const GLdouble SceneView::NEARCLIP = EYEPOINT - 10.0;
const GLdouble SceneView::FARCLIP  = EYEPOINT + 10.0;

/**
 * Constructor and Destructor
 */
SceneView::SceneView() : mx(0.0f), my(0.0f), scl(1.0) {
    currQ[0] = 0.0f; currQ[1] = 0.0f; currQ[2] = 0.0f; currQ[3] = 1.0f;
    
    Glib::RefPtr<Gdk::GL::Config> config = Gdk::GL::Config::create(Gdk::GL::MODE_RGB   |
                                                                   Gdk::GL::MODE_DEPTH |
                                                                   Gdk::GL::MODE_DOUBLE);
    if (!config) {
        config = Gdk::GL::Config::create(Gdk::GL::MODE_RGB |
                                         Gdk::GL::MODE_DEPTH);
        if (!config) {
            exit(-1);
        }
    }
    set_gl_capability(config);
    add_events(Gdk::BUTTON_PRESS_MASK |
               Gdk::BUTTON1_MOTION_MASK |
               Gdk::SCROLL_MASK);
}

SceneView::~SceneView() {
}

/**
 * Update the scene view
 * @param subject
 */
void SceneView::update(const Subject* subject) {
    vtcs.clear();
    Polygon* ply = ((GraphicsModel*)subject)->polygon();
    if (ply && ply->isValid()) {
        // get the point cloud and the indexes of the polygon surfaces
        vtcs = ply->normalizedVertices();
        faceVtxIdxs = ply->faceVertexIndexes();
        get_window()->invalidate_rect(get_allocation(), false);
    }
}

/**
 * On realize
 */
void SceneView::on_realize() {
    Gtk::DrawingArea::on_realize();
    Glib::RefPtr<Gdk::GL::Drawable> drawable = get_gl_drawable();
    if (!drawable->gl_begin(get_gl_context())) {
        return;
    }
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClearDepth(1.0);    
    glDisable(GL_LIGHTING); // disable lighting
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    drawable->gl_end();
}

/**
 * On configure event
 * @param event
 * @return 
 */
bool SceneView::on_configure_event(GdkEventConfigure* event) {
    Glib::RefPtr<Gdk::GL::Drawable> drawable = get_gl_drawable();
    if (!drawable->gl_begin(get_gl_context())) {
        return false;
    }
    GLsizei w = get_width();
    GLsizei h = get_height();
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // orthogonal view
    GLdouble aspect = (GLdouble)w / (GLdouble)h;
    if (h < w) {
        glOrtho(-aspect, aspect, -1.0, 1.0, NEARCLIP, FARCLIP);
    } else {
        aspect = 1.0 / aspect;
        glOrtho(-1.0, 1.0, -aspect, aspect, NEARCLIP, FARCLIP);
    }
    glMatrixMode(GL_MODELVIEW);
    drawable->gl_end();
    return true;
}

/**
 * On expose event
 * @param event
 * @return 
 */
bool SceneView::on_expose_event(GdkEventExpose* event) {
    Glib::RefPtr<Gdk::GL::Drawable> drawable = get_gl_drawable();
    if (!drawable->gl_begin(get_gl_context())) {
        return false;
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glPushMatrix();
    glTranslated(0.0, 0.0, -EYEPOINT);  // set the eye point
    glScaled(scl, scl, scl);            // scale the polygon
    float m[4][4];
    build_rotmatrix(m, currQ);
    glMultMatrixf(&m[0][0]);            // rotate the polygon
    
    // render the point cloud, the polygon edges and surfaces
    renderPointCloud();
    renderEdges();
    renderSurface();

    glPopMatrix();
    if (drawable->is_double_buffered()) {
        drawable->swap_buffers();
    } else {
        glFlush();
    }
    drawable->gl_end();
    return true;
}

/**
 * On mouse button press event
 * @param event
 * @return 
 */
bool SceneView::on_button_press_event(GdkEventButton* event) {
    mx = (float)event->x;
    my = (float)event->y;
    return true;
}

/**
 * On mouse motion notify event
 * Rotate the 3D polygon by the quaternion
 * @param event
 * @return 
 */
bool SceneView::on_motion_notify_event(GdkEventMotion* event) {
    float cx = (float)event->x;
    float cy = (float)event->y;
    if (event->state & GDK_BUTTON1_MASK) {
        float w = (float)this->get_width();
        float h = (float)this->get_height();
        float lastQ[4];
        trackball(lastQ, (2.0f * mx - w) / w,
                         (h - 2.0f * my) / h,
                         (2.0f * cx - w) / w,
                         (h - 2.0f * cy) / h);
        add_quats(lastQ, currQ, currQ);
        get_window()->invalidate_rect(get_allocation(), false);
    }
    mx = cx;
    my = cy;
    return true;
}

/**
 * On mouse wheel scroll event
 * Zoom in/out the 3D polygon
 * @param event
 * @return 
 */
bool SceneView::on_scroll_event(GdkEventScroll* event) {
    switch (event->direction) {
        case GDK_SCROLL_UP:
            scl += 0.05;
            break;
        case GDK_SCROLL_DOWN:
            scl -= 0.05;
            if (scl < 0.2) {
                scl = 0.2;
            }
            break;
        default:
            break;
    }
    get_window()->invalidate_rect(get_allocation(), false);
    return true;
}

/**
 * Render the 3D point cloud
 */
void SceneView::renderPointCloud() {
    glPushMatrix();
    glPointSize(1.0f);
    glBegin(GL_POINTS);
    for_each(vtcs.begin(), vtcs.end(), [](const Vertex& vtx) {
        glColor3dv(vtx.color3d());
        glVertex3dv(vtx.position3d());
    });
    glEnd();
    glPopMatrix();
}

/**
 * Render the edges of the 3D polygon
 */
void SceneView::renderEdges() {
    glPushMatrix();
    glLineWidth(1.0f);
    for_each(faceVtxIdxs.begin(), faceVtxIdxs.end(), [&](const vector<uint>& vtxIdxs) {
        glBegin(GL_LINE_LOOP);
        for_each(vtxIdxs.begin(), vtxIdxs.end(), [&](uint vtxIdx) {
            Vertex vtx = vtcs[vtxIdx];
            glColor3dv(vtx.color3d());
            glVertex3dv(vtx.position3d());
        });
        glEnd();
    });
    glPopMatrix();
}

/**
 * Render the surfaces of the 3D polygon
 */
void SceneView::renderSurface() {
    glPushMatrix();
    for_each(faceVtxIdxs.begin(), faceVtxIdxs.end(), [&](const vector<uint>& vtxIdxs) {
        glBegin(GL_TRIANGLES);
        for_each(vtxIdxs.begin(), vtxIdxs.end(), [&](uint vtxIdx) {
            Vertex vtx = vtcs[vtxIdx];
            glColor3dv(vtx.color3d());
            glVertex3dv(vtx.position3d());
        });
        glEnd();
    });
    glPopMatrix();
}

