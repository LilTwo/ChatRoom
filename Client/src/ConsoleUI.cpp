#include "ConsoleUI.hpp"

wstring ConsoleUI::input_from_user(){
    wstring line;
    getline(wcin,line);
    return line;
}

void ConsoleUI::output_to_user(const wstring& message){
    if(!message.empty()) wcout << message << endl;
}

void ConsoleUI::notify(const wstring &text){
    wcout << text << endl;
}

void ConsoleUI::disconnect(){
    wcout << L"server disconnected" << endl;
}

ConsoleUI::~ConsoleUI(){
    
}
