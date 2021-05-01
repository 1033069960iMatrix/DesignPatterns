//命令模式
class Command{
public:
    virtual ~Command();
    virtual void Execute() = 0;
protected:
    Command();
}

class OpenCommand : public Command{
public:
    OpenCommand(Application *app);
    virtual void Execute() override;
protected:
    virtual const char *AskUser();
private:
    Application *app;
    char *response;
}

class PasteCommand : public Command{
public:
    PasteCommand(Document *);
    virtual void Execute() override;
private:
    Document *doc;
}

template<typename Receiver>
class SimpleCommand : public Command{
public:
    typedef void (Receiver:: *Action)();
    SimpleCommand(Receiver *r, Action a) :
        receiver(r), action(a){}
    virtual void Execute() override;
private:
    Action *action;
    Receiver *receiver;
}
template<typename Receiver>
void SimpleCommand<Receiver>::Execute(){
    (receiver->*action)();
}
//macroCommand管理一个子序列
class MacroCommand : public Command{
public:
    MacroCommand();
    virtual ~MacroCommand();
    virtual void Add(Command *c);
    virtual void Remove(Command *c);
    virtual void Execute() override;
private:
    List<Command *> *cmds;
}