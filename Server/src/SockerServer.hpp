#ifndef SockerServer_hpp
#define SockerServer_hpp

#include <string>
#include <iostream>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <regex>

#ifndef _WIN32
#include <arpa/inet.h>
#include <thread>
#include <vector>
#include <algorithm>
#include <sys/socket.h>
#include <unistd.h>
#include <mutex>
#define SOCKET int
#endif

using std::unordered_map;
using std::unordered_set;
using std::function;
using std::string;
using std::wstring;
using std::wregex;
using std::cout;
using std::endl;
using std::wcout;
using std::thread;

class SocketServer;

typedef function<void(SocketServer&,wstring& message,const wstring& speaker)> handler;

class SocketServer{
public:
    SocketServer(int port,const std::string& host = "localhost");
    void accept_loop();
    void public_message(const std::wstring& message);
    void private_message(const std::wstring& message,const wstring& speaker);
    void private_confirm(const wstring& target,const wstring& speaker);
    ~SocketServer();
private:
    static wregex prefix_pattern;
    SOCKET server;
    SOCKET private_client;
    void on_new_client(SOCKET client);
    void on_client_leave(SOCKET client);
    void recv_loop();
    void on_public_message();
    void create_private_client(struct sockaddr_in server_addr);
    void register_new_client(wstring &name,SOCKET client);
    void register_psuedo_client(SOCKET client);
    unordered_map<wstring,SOCKET> name2socket;
    unordered_map<SOCKET,wstring> socket2name;
    static unordered_map<wstring,handler> prefix2handler;
    fd_set recv_set;
    int max_len;
    std::mutex mtx;
};

#endif /* SockerServer_hpp */
