#pragma once

#include <gtkmm.h>

class save_dialog : public Gtk::FileChooserDialog
{
public:
    save_dialog(const std::string& title);
};
