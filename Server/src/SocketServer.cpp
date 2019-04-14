#include "SocketServer.hpp"

const int SocketServer::max_len = 1024;

void my_send(SOCKET client,const wstring &message){
    wcout << L"my_send " << message << endl;
    uint32_t count = message.length()*sizeof(message[0]);
    cout << "count " << count << endl;
    wcout << L"sended message" << message<<endl;
    count = htonl(count);
    send(client,(char*)&count,4,0);
    send(client, (char*)message.c_str(),message.length()*sizeof(message[0]),0);
}

SocketServer::SocketServer(int port,const string& host):port(port){
#ifdef _WIN32
	WSAData wsaData;
	WORD version = MAKEWORD(2, 2);
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		std::cerr << "initailze error" << endl;
		exit(-1);
	}
#endif
    struct sockaddr_in server_addr;
    memset(&server_addr,0,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    
    server = socket(PF_INET,SOCK_STREAM, 0);
    int option = 1;
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, (char*)&option, sizeof(option));
    if (bind(server,(const struct sockaddr *)&server_addr,sizeof(server_addr)) < 0) {
        std::cerr << "bind error" << std::endl;
        exit(-1);
    }
    
    if (listen(server,SOMAXCONN) < 0) {
        std::cerr << "listen error" << std::endl;
        exit(-1);
    }
    
    FD_ZERO(&recv_set);
    std::thread t_accept(&SocketServer::accept_loop,this);
    t_accept.detach();
    create_private_client(server_addr);
}

wregex SocketServer::prefix_pattern = wregex(L"<(.*?)>.*");

void SocketServer::public_message(const std::wstring& message){
    mtx.lock();
    std::cout << "public message"<<std::endl;
    wstring complete_message = wstring(L"<message>") + message;
    std::for_each(name2socket.begin(),name2socket.end(),[complete_message](auto name_socket){my_send(name_socket.second,complete_message);});
    mtx.unlock();
}

void SocketServer::private_message(const std::wstring& message,const wstring& speaker){
    //<private><target>message
    mtx.lock();
    std::wsmatch match;
    regex_search(message.cbegin()+9,message.cend(),match,prefix_pattern);
    wcout << message << endl;
    wstring target = match[1];
    if(name2socket.find(target) == name2socket.end()) my_send(name2socket[speaker],L"disconnected");
    
    auto displace = match[1].second - message.cbegin() + 1;
    cout << "disaplce:" << displace << endl;
    // <message><private user> says:
    wstring complete_message = wstring(L"<message><private ") + speaker +L">" + wstring(L" says:") + message.substr(displace);
    wcout << complete_message << endl;
    my_send(name2socket[target],complete_message);
    mtx.unlock();
}


void SocketServer::create_private_client(struct sockaddr_in server_addr){
    SOCKET private_server = socket(PF_INET,SOCK_STREAM, 0);
	auto *localhost = "127.0.0.1";
	struct sockaddr_in locahost_addr;
	locahost_addr.sin_family = AF_INET;
	locahost_addr.sin_port = htons(port);
	inet_pton(AF_INET, localhost, &(locahost_addr.sin_addr));

	auto result = connect(private_server, (struct sockaddr *)&locahost_addr, sizeof(locahost_addr));
    if (result < 0) {
		std::cerr << "connect error" << endl;
        exit(-1);
    }
    char buff[1024];
    while(true){
        recv(private_server,buff,max_len,0);
        send(private_server, (char*)L"psuedo client",max_len,0);
    }
}

void SocketServer::accept_loop(){
    while(true){
        cout<<"accepting"<<endl;
        SOCKET client = accept(server,NULL,NULL);
        thread t([](auto this_ptr,SOCKET client){this_ptr->on_new_client(client);},this,client);
        t.detach();
        cout<<"accepted"<<endl;
    }
}

void SocketServer::recv_loop(){
    cout << "enter recv_loop" << endl;
    while(true){
        select(FD_SETSIZE,&recv_set,NULL,NULL,NULL);
        for(auto name_socket:name2socket){
            SOCKET client = name_socket.second;
            if(FD_ISSET(client,&recv_set)){
                wchar_t buff[1024];
                memset(buff,0,1024);
                auto r = recv(client, (char*)buff,1024,0);
                cout << "got message" << endl;
                if(r==0){
                    on_client_leave(client);
                    break;
                }
                std::wstring message(buff);
                std::wsmatch match;
                std::regex_match(message,match,prefix_pattern);
                if(match.size() < 2){
                    wcout << L"no matched prefix to "<<message << endl;
                    break;
                }
                wcout << L"got prefix:"<<match[1]<<endl;
                prefix2handler[match[1]](*this,message,socket2name[client]);
            }
        }
        std::for_each(name2socket.begin(),name2socket.end(),[this](auto name_socket){FD_SET(name_socket.second,&this->recv_set);});
    }
}

void SocketServer::register_new_client(wstring &name,SOCKET client){
    name2socket[name] = client;
    socket2name[client] = name;
    FD_SET(client,&recv_set);
    cout << "user name" <<name.c_str()<<" accepted" << endl;
    my_send(client,L"confirmed");
    public_message(name + L" has entered the room");
    send(private_client,"1",1,0);
    return;
}

void SocketServer::register_psuedo_client(SOCKET client){
    private_client = client;
    cout<<"psuedo client registered"<< endl;
    private_client = client;
    FD_SET(client,&recv_set);
    std::thread t_recv(&SocketServer::recv_loop,this);
    t_recv.detach();
    return;
}

void SocketServer::on_new_client(SOCKET client){
    std::wstring message(L"username");
    my_send(client,message);
    wchar_t buff[max_len];
    memset(buff,0,max_len);
    while(true){
        std::cout << "recving" << std::endl;
        auto r = recv(client,(char*)buff,max_len,0);
        if( r == 0){
            cout << "disconnected during username"<<endl;
            return;
        }
        std::wstring name(buff);
        wcout << L"received " << name << endl;
        if(name == wstring(L"psuedo client")){
            register_psuedo_client(client);
            return;
        }
        if(name2socket.find(name) == name2socket.end()){
            register_new_client(name,client);
            return;
        }
        else{
            my_send(client,L"conflict");
            std::cout << "user name refused" <<std::endl;
        }
    }
}

void SocketServer::on_client_leave(SOCKET client){
    cout << "client leave" << endl;
    wstring name = socket2name[client];
    socket2name.erase(socket2name.find(client));
    name2socket.erase(name2socket.find(name));
    public_message(name + wstring(L" has leaved the room"));
}

void SocketServer::private_confirm(const wstring& target,const wstring& speaker){
    SOCKET client = name2socket[speaker];
    wcout << L"confirming "<<target<<endl;
    if(name2socket.find(target) != name2socket.end()) return my_send(client, L"confirmed");
    else return my_send(client, L"no user");
}

SocketServer::~SocketServer(){
    close(server);
}