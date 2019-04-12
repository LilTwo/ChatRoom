#include "SocketClient.hpp"
#include <string>
#include <iostream>
#include <memory>
#include <regex>
#include <locale>
#include <codecvt>
#include "UI.hpp"

using namespace std;


int main(){
    SocketClient socket_client(&UI::get_console_ui);
}

