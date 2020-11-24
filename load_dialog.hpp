#pragma once

#include <gtkmm.h>

class load_dialog : public Gtk::FileChooserDialog
{
public:
    load_dialog(const std::string& title);
};
