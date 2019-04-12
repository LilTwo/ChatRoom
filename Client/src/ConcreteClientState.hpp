#ifndef ConcreteClientState_hpp
#define ConcreteClientState_hpp

#include "ClientState.hpp"
#include <string>
using namespace std;

class PublicMessageState:public ClientState{
public:
    PublicMessageState(Context& context);
    virtual wstring process_send_impl(const wstring& message);
    virtual wstring ui_text();
    
protected:
    virtual wstring process_recv_impl(const wstring& message);
private:
    wstring target;
};

class PrivateMessageState:public ClientState{
public:
    PrivateMessageState(Context& context,const wstring& target);
    virtual wstring process_send_impl(const wstring& message);
    virtual wstring ui_text();
    
protected:
    virtual wstring process_recv_impl(const wstring& message);
private:
    wstring target;
    wstring prefix;
};

class UserNameState:public ClientState{
public:
    UserNameState(Context& context);
    virtual wstring process_send_impl(const wstring& message);
    virtual wstring ui_text();
    
protected:
    virtual wstring process_recv_impl(const wstring& message);
};

#endif
