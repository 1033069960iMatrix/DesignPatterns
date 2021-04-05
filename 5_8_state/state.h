//状态  对象行为型模式

class TCPOctetStream;
class TCPState;
class TCPConnection {
public:
    TCPConnection();

    void ActiveOpen();
    void PassiveOpen();
    void Close();
    void Send();
    void Acknowledge();
    void Synchronize();

    void ProcessOctet(TCPConnection *connect);
private:
    friend class TCPState;
    void ChangeState(TCPState *state);
    TCPState *state;
}

class TCPState {
public:
    virtual void Transmit(TCPConnection *connect, TCPOctetStream *stream);
    virtual void ActiveOpen(TCPConnection *connect);
    virtual void PassiveOpen(TCPConnection *connect);
    virtual void Close(TCPConnection *connect);
    virtual void Synchronize(TCPConnection *connect);
    virtual void Acknowledge(TCPConnection *connect);
    virtual void Send(TCPConnection *connect);
protected:
    void ChangeState(TCPConnection *connect, TCPState *state);
}

class TCPEstablished : public TCPState {
public:
    static TCPState *Instance();
    virtual void Transmit(TCPConnection *connect, TCPOctetStream *stream);
    virtual void Close(TCPConnection *connect);
}

class TCPListen : public TCPState {
public:
    static TCPState *Instance();
    virtual void Send(TCPConnection *connect);
    //...
}

class TCPClosed : public TCPState {

}