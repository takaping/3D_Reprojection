/* 
 * MpoFileDialog Class
 *  - The dialog to choose the MPO file
 * 
 * File:   MpoFileDialog.h
 * Author: munehiro
 *
 * Created on January 31, 2014, 10:01 PM
 */

#ifndef MPOFILEDIALOG_H
#define	MPOFILEDIALOG_H

#include <gtkmm-2.4/gtkmm.h>

class MpoFileDialog : public Gtk::FileChooserDialog {
public:
    MpoFileDialog(Gtk::Window& parent, const Glib::ustring& title, Gtk::FileChooserAction action=Gtk::FILE_CHOOSER_ACTION_OPEN);
    virtual ~MpoFileDialog();
private:

};

#endif	/* MPOFILEDIALOG_H */

