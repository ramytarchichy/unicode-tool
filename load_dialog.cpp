#include "load_dialog.hpp"

load_dialog::load_dialog(const std::string& title): Gtk::FileChooserDialog(title, Gtk::FILE_CHOOSER_ACTION_OPEN)
{
    //Add response buttons the the dialog:
    add_button("_Cancel", Gtk::RESPONSE_CANCEL);
    add_button("_Open", Gtk::RESPONSE_OK);

    //Add filters, so that only certain file types can be selected:
    auto filter_text = Gtk::FileFilter::create();
    filter_text->set_name("Text files");
    filter_text->add_mime_type("text/plain");
    add_filter(filter_text);

    auto filter_any = Gtk::FileFilter::create();
    filter_any->set_name("Any files");
    filter_any->add_pattern("*");
    add_filter(filter_any);
}
