#ifndef UI_hpp
#define UI_hpp
#include <string>
#include <memory>
using namespace std;

class UI{
public:
    virtual wstring input_from_user(const wstring &ui_text)=0;
    virtual void output_to_user(const wstring& message)=0;
    static unique_ptr<UI> get_console_ui();
    virtual void notify(const wstring &text)=0;
    virtual void disconnect()=0;
    virtual ~UI()=0;
};

#endif /* UI_hpp */
