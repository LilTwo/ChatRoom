#ifndef SockerServer_hpp
#define SockerServer_hpp

#include <string>
#include <iostream>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <regex>
#include <mutex>
#include <memory>
#include <thread>
#include <vector>
#include <algorithm>
#include <locale>
#include <codecvt>

#ifndef _WIN32
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#define SOCKET int
#define C(x) x
#else
#include <winsock2.h>
#include <Ws2tcpip.h>
#define close closesocket
#define C(x) L##x
#endif

#include "Context.hpp"

using namespace std;

class SocketServer;
class ClientState;
class UI;

typedef unique_ptr<ClientState> state_ptr;
typedef function<void(SocketServer&,wstring&,const wstring& speaker)> handler;
typedef std::vector<SOCKET> socket_vect;
typedef unique_ptr<UI> ui_ptr;
typedef std::function<ui_ptr(void)> t_ui_getter;

class SocketClient:public Context{
public:
    SocketClient(const t_ui_getter& ui_getter,int port=3000);
    virtual void set_state(state_ptr state);
    ~SocketClient();

private:
    static const int max_len;
    void recv_loop();
    void send_loop();
    state_ptr state;
    SOCKET server;
    mutex mtx;
    ui_ptr ui;
};

#endif /* SockerServer_hpp */
