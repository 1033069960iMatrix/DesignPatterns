#include "state.h"
TCPConnection::TCPConnection() {
    state = TCPClosed::Instance();
}
void TCPConnection::ChangeState(TCPState *state) {
    this->state = state;
}
void TCPConnection::ActiveOpen() {
    state->ActiveOpen(this);
}
void TCPConnection::PassiveOpen() {
    state->PassiveOpen(this);
}
void TCPConnection::Close() {
    state->Close(this);
}
void TCPConnection::Acknowledge() {
    state->Acknowledge(this);
}
void TCPConnection::Synchronize() {
    state->Synchronize(this);
}

void TCPState::Transmit(TCPConnection *connect, TCPOctetStream *stream){}
void TCPState::ActiveOpen(TCPConnection *connect){}
void TCPState::PassiveOpen(TCPConnection *connect){}
void TCPState::Close(TCPConnection *connect){}
void TCPState::Synchronize(TCPConnection *connect){}
void TCPState::ChangeState(TCPConnection *connect, TCPState *state) {
    connect->ChangeState(state);
}

void TCPCLosed::ActiveOpen(TCPConnection *connect){
    //..send SYN, receive SYN, ACK, etc.
    ChangeState(connect, TCPEstablished:Instance());
}

void TCPEstablished::Transmit(TCPConnection *connect, TCPOctetStream *stream) {
    connect->ProcessOctet(stream);
}