#ifndef ConsoleUI_hpp
#define ConsoleUI_hpp

#include <iostream>
#include <string>
using namespace std;
#include "UI.hpp"

class ConsoleUI:public UI{
public:
    virtual wstring input_from_user(const wstring &ui_text);
    virtual void output_to_user(const wstring& message);
    virtual void notify(const wstring &text) ;
    virtual void disconnect();
    ~ConsoleUI();
};

#endif /* ConsoleUI_hpp */
