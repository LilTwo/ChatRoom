#include "SocketClient.hpp"
#include "ClientState.hpp"
#include "UI.hpp"


const int SocketClient::max_len = 1024;

string wstring2string(const wstring& w){
    using convert_type = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_type, wchar_t> converter;
    return converter.to_bytes(w);
}

void SocketClient::set_state(state_ptr state){
    this->state = move(state);
    ui->notify(this->state->ui_text());
}

void SocketClient::send_loop(){
    wstring line;
    while(true){
        wstring message = ui->input_from_user();
        mtx.lock();
        message = state->process_send(message);
        mtx.unlock();
        
        send(server, (char*)message.c_str(), message.length()*sizeof(message[0]), 0);
    }
}

void SocketClient::recv_loop(){
    wchar_t buf[max_len];
    uint32_t count_buf[1];
    while(true){
        memset(count_buf,0,4);
        memset(buf,0,max_len);
        auto r = recv(server, (char*)count_buf,4,0);
        if(r == 0){
            ui->disconnect();
            break;
        }
        uint32_t count = ntohl(count_buf[0]);
        recv(server,(char*)buf,count,0);
        wstring message(buf);
        
        mtx.lock();
        wstring response = state->process_recv(message);
        mtx.unlock();
        ui->output_to_user(response);
    }
}

SocketClient::SocketClient(const t_ui_getter& ui_getter,int port){
#ifdef _WIN32
	WSAData wsaData;
	WORD version = MAKEWORD(2, 2);
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		std::cerr << "initailze error" << endl;
		exit(-1);
	}
#endif

    ui = ui_getter();
    ui->output_to_user(L"enter server ip(leave blank for localhost):");
    //wstring ip = ui->input_from_user();
    //const char *server_ip;
    //if(ip.empty()) server_ip = C("127.0.0.1");
    //else const char *server_ip = wstring2string(ip).c_str();
	auto *server_ip = "127.0.0.1";
    
    ui->notify(L"connecting");
    
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
	inet_pton(AF_INET, server_ip, &(server_addr.sin_addr));
    
    server = socket(PF_INET,SOCK_STREAM,0);
    if (server < 0) {
        perror("socket");
        exit(-1);
    }
    
    if (connect(server,(const struct sockaddr *)&server_addr,sizeof(server_addr)) < 0) {
        perror("connect");
        //exit(-1);
    }
    
    set_state(ClientState::get_initial_state(*this));
    thread t_recv = thread(&SocketClient::recv_loop,this);
    t_recv.detach();
    send_loop();
}

SocketClient::~SocketClient(){
    close(server);
}
