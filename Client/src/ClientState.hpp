
#ifndef ClientState_hpp
#define ClientState_hpp
#include <string>
#include <iostream>
#include "Context.hpp"
using namespace std;
class ClientState;
typedef unique_ptr<ClientState> state_ptr;

class ClientState{
public:
    ClientState(Context &context);
    wstring process_send(const wstring& message);
    wstring process_recv(const wstring& message);
    static state_ptr get_initial_state(Context &context);
    virtual wstring ui_text()=0;
    virtual ~ClientState(){};
protected:
    Context &context;
    virtual wstring process_send_impl(const wstring& message)=0;
    virtual wstring process_recv_impl(const wstring& message)=0;
    wstring response_private;
};
#endif /* ClientState_hpp */
