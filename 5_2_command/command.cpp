#include "command.h"
OpenCommand::OpenCommand(Application *a){
    app = a;
}
void OpenCommand::Execute(){
    const char *name = AskUser();
    if(name != NULL){
        Document *doc = new Document(name);
        app->add(doc);
        doc->Open();
    }
}

PasteCommand::PasteCommand(Document *d){
    doc = d;
}
void PasteCommand::Execute(){
    doc->Paste();
}
//调用SimpleCommand方法
MyClass *receiver = new MyClass;
Command *aCommand = new SimpleCommand<MyClass>(receiver, &MyClass::Action);

void Macro::Execute(){
    ListIterator<Command *> i(cmds);
    for(i.First(); i != i.end(); i.Next()){
        Command *c = i.CurrentItem();
        c->Execute();
    }
}
void MacroCommand::Add(Command *c){
    cmds->Append(c);
}
void MacroCommand::Remove(Command *c){
    cmds->Remove(c);
}