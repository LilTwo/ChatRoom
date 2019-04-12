#include "SocketClient.hpp"
#include "ClientState.hpp"
#include "UI.hpp"

int SocketClient::max_len = 1024;

void SocketClient::set_state(state_ptr state){
    this->state = move(state);
    ui->notify(this->state->ui_text());
}

void SocketClient::send_loop(){
    wstring line;
    while(true){
        wstring message = ui->input_from_user(state->ui_text());
        mtx.lock();
        message = state->process_send(message);
        mtx.unlock();
        
        send(server, message.c_str(), message.length()*sizeof(message[0]), 0);
    }
}

void SocketClient::recv_loop(){
    wchar_t buf[max_len];
    uint32_t count_buf[0];
    while(true){
        memset(count_buf,0,4);
        memset(buf,0,max_len);
        auto r = recv(server,count_buf,4,0);
        if(r == 0){
            ui->disconnect();
            break;
        }
        uint32_t count = ntohl(count_buf[0]);
        recv(server,buf,count,0);
        wstring message(buf);
        
        mtx.lock();
        wstring response = state->process_recv(message);
        mtx.unlock();
        ui->output_to_user(response);
    }
}

SocketClient::SocketClient(const t_ui_getter& ui_getter,int port,const char *server_ip){
    ui = ui_getter();
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
        exit(-1);
    }
    
    set_state(ClientState::get_initial_state(*this));
    thread t_recv = thread(&SocketClient::recv_loop,this);
    t_recv.detach();
    send_loop();
}

SocketClient::~SocketClient(){
    close(server);
}
