#ifndef DATA_PARSER
#define DATA_PARSER
#include <list>
#include <functional>
#include <vector>
#include <string>
#include <map>
#include <mutex>
//环形缓冲区设计 若溢出后来数据会清除原先的
class DataStorage {
public:
    DataStorage(int size);
    ~DataStorage();
    void PutData(uint8_t data);
    void PutData(uint8_t *data, int length);
    void PopData(uint8_t *data, int length);
    void PopData();
    uint8_t GetData(int idx);
    //当前缓存的未处理字节数
    int CurrentBufSize();
private:
    const int QUEUE_SIZE;
    uint8_t *parserQueue; // 数据解析队列
    int checkIdx = 0;
    int writeIdx = 0;
    int currentVaildCache = 0;
};
/* 解析器结构 */
class DataParser {
public:
    static DataParser *GetInstance(std::string type);
    virtual ~DataParser();
    void PutData(uint8_t data);
    void PutData(uint8_t *data, int length);
    bool PopFrame(uint8_t *data, int &length);
protected:
    virtual int GetFrameSize() = 0;
    virtual void SetHeaderAndTail() = 0;
    virtual bool check() = 0;
    DataParser();
    uint8_t GetData(int idx);
    static std::map<std::string, DataParser *> registry;
    uint8_t header[10] = { 0 };
    int headerLength = 0;
    uint8_t tail[10] = { 0 };
    int tailLength = 0;
private:
    bool SearchHeader();
    std::recursive_mutex mutex1;
    std::condition_variable bufferNotEmpty;
    std::condition_variable bufferNotFull;
    static DataParser *parser;
    //缓冲接收数据用
    DataStorage storage;
};

class ConcreteParser : public DataParser {
protected:
    /**
     * 从帧头算起，获取当前帧长度
     *
     *
     * @return: 长度
     */
    virtual int GetFrameSize() override;
    /**
     * 设置帧头和帧尾
     *
     *
     * @return: 
     */
    virtual void SetHeaderAndTail() override;
    /**
     * 校验帧算法
     *
     *
     * @return:
     */
    virtual bool check() override;
private:
    ConcreteParser();
    static ConcreteParser *self;
};
#endif
