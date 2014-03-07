/* 
 * RigDialog Class
 *  - The dialog to edit parameters of the calibration rig.
 *  - Gtk::Dialog is inherited.
 *  - The layout of the dialog is configured by the "RigDialog.glade" file.
 * 
 * File:   RigDialog.cpp
 * Author: munehiro
 * 
 * Created on March 6, 2014, 8:13 PM
 */

#include "RigDialog.h"

/**
 * Constructor and Destructor
 * @param object
 * @param builder
 */
RigDialog::RigDialog(BaseObjectType* object, const Glib::RefPtr<Gtk::Builder>& builder)
: Gtk::Dialog(object) {
    set_modal(true);    // set the modal
    // relate between the widget and the variable
    Gtk::Button *okButton, *cancelButton;
    builder->get_widget("cb_ptn_button", cbPtnButton);
    builder->get_widget("cg_ptn_button", cgPtnButton);
    builder->get_widget("row_spin", rowSpin);
    builder->get_widget("col_spin", colSpin);
    builder->get_widget("dist_spin", distSpin);
    builder->get_widget("ok_button", okButton);
    builder->get_widget("cancel_button", cancelButton);
    // set the parameters of the spin button
    rowSpin->set_range(3.0, 1000.0);
    rowSpin->set_increments(1.0, 10.0);
    colSpin->set_range(3.0, 1000.0);
    colSpin->set_increments(1.0, 10.0);
    distSpin->set_range(5.0, 1000.0);
    distSpin->set_increments(0.1, 1.0);
    // set the signal and the slot
    okButton->signal_clicked().connect(sigc::mem_fun(*this, &RigDialog::ok));
    cancelButton->signal_clicked().connect(sigc::mem_fun(*this, &RigDialog::cancel));
}

RigDialog::~RigDialog() {
}

/**
 * Set the type of the calibration rig pattern
 * @param the type of the pattern
 */
void RigDialog::setPattern(Pattern ptn) {
    if (ptn == Pattern::Chessboard) {
        cbPtnButton->set_active(true);
        cgPtnButton->set_active(false);
    } else if (ptn == Pattern::CircleGrid) {
        cbPtnButton->set_active(false);
        cgPtnButton->set_active(true);
    } else {
        throw string("Invalid calibration pattern");
    }
}

/**
 * Set the number of rows on the calibration rig pattern
 * @param number of rows
 */
void RigDialog::setRows(int rows) {
    double min, max;
    rowSpin->get_range(min, max);
    if (rows < min) {
        rows = (int)min;
    } else if (rows > max) {
        rows = (int)max;
    }
    rowSpin->set_value(rows);
}

/**
 * Set the number of columns on the calibration rig pattern
 * @param number of rows
 */
void RigDialog::setColumns(int cols) {
    double min, max;
    colSpin->get_range(min, max);
    if (cols < min) {
        cols = (int)min;
    } else if (cols > max) {
        cols = (int)max;
    }
    colSpin->set_value(cols);
}

/**
 * Set the distance between corners or centers on the calibration rig pattern
 * @param distance between corners or centers
 */
void RigDialog::setDistance(double dist) {
    double min, max;
    distSpin->get_range(min, max);
    if (dist < min) {
        dist = min;
    } else if (dist > max) {
        dist = max;
    }
    distSpin->set_value(dist);
}

/**
 * OK button press event
 */
void RigDialog::ok() {
    response(Gtk::RESPONSE_OK);
    hide();
}

/**
 * Cancel button press event
 */
void RigDialog::cancel() {
    response(Gtk::RESPONSE_CANCEL);
    hide();
}

