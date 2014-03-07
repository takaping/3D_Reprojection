/* 
 * RigDialog Class
 *  - The dialog to edit parameters of the calibration rig.
 *  - Gtk::Dialog is inherited.
 *  - The layout of the dialog is configured by the "RigDialog.glade" file.
 * 
 * File:   RigDialog.h
 * Author: munehiro
 *
 * Created on March 6, 2014, 8:13 PM
 */

#ifndef RIGDIALOG_H
#define	RIGDIALOG_H

#include <gtkmm-2.4/gtkmm.h>

using namespace std;

class RigDialog : public Gtk::Dialog {
public:
    // the type of the pattern on the calibration rig
    enum struct Pattern : int {
        Chessboard, // chessboard pattern
        CircleGrid  // circle grid pattern
    };
    RigDialog(BaseObjectType* object, const Glib::RefPtr<Gtk::Builder>& builder);
    RigDialog(const RigDialog& orig);
    virtual ~RigDialog();
    // get the type of the pattern
    Pattern pattern() const {
        if (cbPtnButton->get_active()) {
            return Pattern::Chessboard;
        } else if (cgPtnButton->get_active()) {
            return Pattern::CircleGrid;
        } else {
            throw string("Invalid calibration pattern");
        }
    };
    // get the number of rows on the calibration rig pattern
    int rows() const { return rowSpin->get_value_as_int(); };
    // get the number of columns on the calibration rig pattern
    int columns() const { return colSpin->get_value_as_int(); };
    // get the distance between corners or centers on the calibration rig pattern
    double distance() const { return distSpin->get_value(); };
    void setPattern(Pattern ptn);
    void setRows(int rows);
    void setColumns(int cols);
    void setDistance(double dist);
private:
    void ok();
    void cancel();
    // radio button for the type of the chessboard and the circle grid pattern
    Gtk::RadioButton *cbPtnButton, *cgPtnButton;
    // spin button for the number of rows, columns and the distance between corners or centers
    Gtk::SpinButton *rowSpin, *colSpin, *distSpin;

};

#endif	/* RIGDIALOG_H */

