#include <iostream>
#include <iomanip>
#include <bitset>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <string>

#include <gtkmm.h>

#include "load_dialog.hpp"
#include "save_dialog.hpp"

std::vector<char> to_utf8(unsigned int code)
{
    if (code <= 0x007F)
        return {
            static_cast<char>(code)
            };
    else if (code <= 0x07FF)
        return {
            static_cast<char>(0b11000000 | (code >> 6) & 0b00011111),
            static_cast<char>(0b10000000 | code & 0b00111111)
            };
    else if (code <= 0xFFFF)
        return {
            static_cast<char>(0b11100000 | (code >> 12) & 0b00001111),
            static_cast<char>(0b10000000 | (code >> 6) & 0b00111111),
            static_cast<char>(0b10000000 | code & 0b00111111)
            };
    else // code <= 0x10FFFF
        return {
            static_cast<char>(0b11110000 | (code >> 18) & 0b00000111),
            static_cast<char>(0b10000000 | (code >> 12) & 0b00111111),
            static_cast<char>(0b10000000 | (code >> 6) & 0b00111111),
            static_cast<char>(0b10000000 | code & 0b00111111)
            };
}

bool is_number(const std::string& s)
{
    return !s.empty() && std::find_if(s.begin(), s.end(), [](char c){
        return !std::isdigit(c);
        }) == s.end();
}

class HelloWorld : public Gtk::Window
{
public:
    HelloWorld(): box_1(Gtk::Orientation::ORIENTATION_VERTICAL), box_2(Gtk::Orientation::ORIENTATION_VERTICAL)
    {
        set_title("Unicode Tool");

        // Sets the border width of the window.
        box_1.set_border_width(10);
        box_2.set_border_width(10);
        box_2.set_spacing(10);

        // When the button receives the "clicked" signal, it will call the
        // on_button_clicked() method defined below.
        entry_code.signal_changed().connect(sigc::mem_fun(*this,
            &HelloWorld::on_entry_changed));

        btn_load_in.signal_clicked().connect(sigc::mem_fun(*this,
            &HelloWorld::on_click_load_in));
            
        btn_load_conv.signal_clicked().connect(sigc::mem_fun(*this,
            &HelloWorld::on_click_load_conv));

        btn_save.signal_clicked().connect(sigc::mem_fun(*this,
            &HelloWorld::on_click_save));

        // This packs the button into the Window (a container).
        add(ntb);
        ntb.add(box_1);
        ntb.add(box_2);
        box_1.add(entry_code);
        box_1.add(label_char);
        box_1.add(label_unicode);
        box_1.add(label_utf8);
        box_2.add(btn_load_in);
        box_2.add(btn_load_conv);
        box_2.add(btn_save);

        // The final step is to display this newly created widget...
        entry_code.show();
        label_char.show();
        label_unicode.show();
        label_utf8.show();
        box_1.show();
        box_2.show();
        btn_load_in.show();
        btn_load_conv.show();
        btn_save.show();
        ntb.show();
    }

    virtual ~HelloWorld()
    {

    }

private:
    //Signal handlers:
    void on_entry_changed()
    {
        if (is_number(entry_code.get_text()))
        {
            unsigned int code = std::stoul(entry_code.get_text());
            if (code <= 0x10FFFF)
            {
                std::stringstream ss;
                ss << "U+" << std::hex << std::setfill('0') << std::setw(4) << std::uppercase << code;
                label_unicode.set_text(ss.str());

                std::vector<char> utf = to_utf8(code);
                std::string a(utf.data(), utf.size());
                label_char.set_text(a);

                std::stringstream ss2;
                for(char n : utf)
                    ss2 << std::hex << std::setfill('0') << std::setw(2) << std::uppercase << static_cast<unsigned short>(static_cast<unsigned char>(n)) << " ";
                label_utf8.set_text(ss2.str());
            }
        }
    }

    void on_click_load_in()
    {
        load_dialog dialog("Input File");

        //Show the dialog and wait for a user response:
        int result = dialog.run();

        //Handle the response:
        switch(result)
        {
            case(Gtk::RESPONSE_OK):
            {
                input_file = std::ifstream(dialog.get_filename());
            }
            break;
        }
    }

    void on_click_load_conv()
    {
        load_dialog dialog("Conversion File");

        //Show the dialog and wait for a user response:
        int result = dialog.run();

        //Handle the response:
        switch(result)
        {
            case(Gtk::RESPONSE_OK):
            {
                std::ifstream conv_file(dialog.get_filename());

                conversion_map = {};
                while(!conv_file.eof())
                {
                    int from;
                    conv_file >> std::dec >> from;

                    unsigned int to;
                    conv_file >> std::hex >> to;
                    
                    conversion_map[from] = to;
                }
            }
            break;
        }
    }

    void on_click_save()
    {
        save_dialog dialog("Save converted file");

        //Show the dialog and wait for a user response:
        int result = dialog.run();

        //Handle the response:
        switch(result)
        {
            case(Gtk::RESPONSE_OK):
            {
                std::ofstream result(dialog.get_filename());

                input_file.seekg(0);
                while(!input_file.eof())
                {
                    char c;
                    input_file >> std::noskipws >> c;

                    if (input_file.eof())
                        break;

                    if (conversion_map.count(c))
                    {
                        const auto translated_char = to_utf8(conversion_map.at(c));
                        for (auto cc : translated_char)
                            result << cc;
                    }
                    else
                    {
                        result << c;
                    }
                }
            }
            break;
        }
    }

    std::unordered_map<unsigned char, unsigned int> conversion_map;
    std::ifstream input_file;

    //Member widgets
    Gtk::Notebook ntb;
    Gtk::Box box_1;
    Gtk::Box box_2;
    Gtk::Entry entry_code;
    Gtk::Label label_char;
    Gtk::Label label_unicode;
    Gtk::Label label_utf8;
    Gtk::Button btn_load_in = Gtk::Button("Load Input File");
    Gtk::Button btn_load_conv = Gtk::Button("Load Conversion File");
    Gtk::Button btn_save = Gtk::Button("Save Result");
};


int main(int argc, char** argv)
{
    auto app = Gtk::Application::create(argc, argv, "ch.tarchi.unicode-tool");
    HelloWorld helloworld;
    //Shows the window and returns when it is closed.
    return app->run(helloworld);
}
