#include "ClientState.hpp"
#include "ConcreteClientState.hpp"

ClientState::ClientState(Context& context):context(context){}

wstring ClientState::process_send(const wstring& message){
    if(message.find(L"/r") == 0 && !response_private.empty()){
        auto private_state = make_unique<PrivateMessageState>(context,response_private);
        wstring message = private_state->process_send_impl(message);
        context.set_state(move(private_state));
        return message;
    }
    else{
        return process_send_impl(message);
    }
}

wstring ClientState::process_recv(const wstring& message){
    if(message.find(L"<private") == 9){
        int sender_begin = 18;
        int sender_end = message.find(L"> says:");
        wstring sender = message.substr(sender_begin,sender_end-sender_begin);
        response_private = sender;
    }
    if(message.find(L"<message>") == 0){
        return message.substr(9);
    }
    else{
        return process_recv_impl(message);
    }
}

PublicMessageState::PublicMessageState(Context& context):ClientState(context){}

wstring PublicMessageState::process_send_impl(const wstring& message){
    if(target.empty() && (message.find(L"/p ") == 0)){
        target = message.substr(3);
        return L"<private confirm>"+message.substr(3);
    }
    return L"<public>" + message;
}

wstring PublicMessageState::ui_text(){
    return L"<public mode>: ";
}

wstring PublicMessageState::process_recv_impl(const wstring& message){
    if(!target.empty() && message == wstring(L"confirmed")){
//        state = make_shared<PrivateMessageState>(context,target);
        context.set_state(make_unique<PrivateMessageState>(context,target));
        return wstring();
    }
    else if (!target.empty()){
        wstring response = target + L" not found";
        target = wstring();
        return response;
    }
    return message;
}

PrivateMessageState::PrivateMessageState(Context &context,const wstring &target):ClientState(context),target(target){
    prefix = wstring(L"<private><")+target+L">";
}

wstring PrivateMessageState::process_send_impl(const wstring &message){
    if(message.find(L"/s") == 0){
        context.set_state(make_unique<PublicMessageState>(context));
        return L"<ignore>";
    }
    return prefix+message;
}

wstring PrivateMessageState::process_recv_impl(const wstring &message){
    if(message == wstring(L"disconnected")){
        context.set_state(make_unique<PublicMessageState>(context));
        return wstring(L" user " + target + L"has diconnected");
    }
    else{
        return message;
    }
}

wstring PrivateMessageState::ui_text(){
    return L"<private mode to "+target+L">:";
}

UserNameState::UserNameState(Context& context):ClientState(context){}

wstring UserNameState::process_send_impl(const wstring& message){
    return message;
}
wstring UserNameState::process_recv_impl(const wstring& message){
    if(message == wstring(L"confirmed")){
        context.set_state(make_unique<PublicMessageState>(context));
        return wstring(L"username confirmed");
    }
    else if(message == wstring(L"conflict")){
        return wstring(L"username conflicted");
    }
    else return wstring();
}

wstring UserNameState::ui_text(){
    return L"enter yor username: ";
}

state_ptr ClientState::get_initial_state(Context &context){
    return make_unique<UserNameState>(context);
}
