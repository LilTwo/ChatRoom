# C++ implementation of multi-user chat room
A multi-user chat room implemented by TCP connection, for recording the usage of C++ Socket API.<br/>
This project is tested on Windows(Visual Studio 2015) and MacOS(XCODE 9), using macro to distinguish differnt platforms.

## Usage
1. Create a server. 
2. Create a client.
3. Enter an username, server will check if it is duplicated.<br/>
4. Clients has two modes: public and private, default is public.<br/>
In public mode, any message will be sended to all user, type `/p <otheruser>` can change to private mode.<br/>
5. In private mode, sended message will only be recevied  `<otheruser>`, type `\s` will back to public mode.
6. `/r` can enter to private mode with the user who just sended a private message to you.
