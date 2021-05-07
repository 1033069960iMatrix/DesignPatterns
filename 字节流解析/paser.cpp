#include "parser.h"
DataStorage::DataStorage(int size) : QUEUE_SIZE(size){
    parserQueue = (uint8_t *)malloc(size);
}
DataStorage::~DataStorage() {

}

void DataStorage::PutData(uint8_t data) {
    if (currentVaildCache == QUEUE_SIZE) {
        PopData();
    }
    parserQueue[writeIdx] = data;
    writeIdx = (writeIdx + 1) % QUEUE_SIZE;
    currentVaildCache++;
}
void DataStorage::PutData(uint8_t *data, int length) {
    if (currentVaildCache + length > (int)QUEUE_SIZE) {
        int needPop = currentVaildCache + length - QUEUE_SIZE;
        for (int i = 0; i < needPop; i++) {
            PopData();
        }
    }

    if (writeIdx + length > QUEUE_SIZE) {
        int tailCnt = QUEUE_SIZE - writeIdx;
        memcpy(&parserQueue[writeIdx], data, tailCnt);
        memcpy(parserQueue, data + tailCnt, length - tailCnt);
    } else {
        memcpy(&parserQueue[writeIdx], data, length);
    }
    writeIdx = (writeIdx + length) % QUEUE_SIZE;
    currentVaildCache += length;
}
void DataStorage::PopData(uint8_t *data, int length) {
    if (checkIdx + length <= QUEUE_SIZE) {
        memcpy(data, parserQueue + checkIdx, length);
    } else {
        int tailCnt = QUEUE_SIZE - checkIdx;
        memcpy(data, parserQueue + checkIdx, tailCnt);
        memcpy(data + tailCnt, parserQueue, length - tailCnt);
    }
    //printf("checkIdx %d\n", checkIdx);
    checkIdx = (checkIdx + length) % QUEUE_SIZE;
    currentVaildCache -= length;
}
uint8_t DataStorage::GetData(int idx) {
    return parserQueue[(checkIdx + idx) % QUEUE_SIZE];
}
int DataStorage::CurrentBufSize() {
    return currentVaildCache;
}
void DataStorage::PopData() {
    if (currentVaildCache > 0) {
        //printf("currentVaildCache %d\n", currentVaildCache);
        checkIdx = (checkIdx + 1) % QUEUE_SIZE;
        currentVaildCache--;
        if (checkIdx == writeIdx) {
            _ASSERT(currentVaildCache == 0);
        }
    }
}

DataParser *DataParser::parser = NULL;
std::map<std::string, DataParser *> DataParser::registry;
DataParser *DataParser::GetInstance(std::string type) {
    if (parser == NULL) {
        auto it = registry.find(type);
        if (it != registry.end()) {
            parser = it->second;
        }
    }
    return parser;
}
DataParser::DataParser() : storage(1000){}
DataParser::~DataParser() {}

/**
 * 将数据添加到解析器中进行解析
 *
 * @data: 要解析的数据
 *
 */
void DataParser::PutData(uint8_t data) {
    storage.PutData(data);
}
/**
 * 将数据添加到解析器中进行解析
 *
 * @data: 要解析的数据
 * @length: 长度
 *
 */
void DataParser::PutData(uint8_t *data, int length) {
    std::lock_guard<std::recursive_mutex> lk(mutex1);
    storage.PutData(data, length);
}
uint8_t DataParser::GetData(int idx) {
    std::lock_guard<std::recursive_mutex> lk(mutex1);
    return storage.GetData(idx);
}
bool DataParser::SearchHeader() {
    bool find = false;
    int i;
    while (1) {
        if (storage.CurrentBufSize() < headerLength) {
            break;
        }
        for (i = 0; i < headerLength; i++) {
            if (GetData(i) != header[i]) {
                break;
            }
        }
        if (i == headerLength) {
            find = true;
            return true;
        } else {
            storage.PopData();
            if (storage.CurrentBufSize() == 0) {
                break;
            }
        }
    }
    return false;
}
/**
 * 解析成功后从解析器中取出解析结果
 *
 *
 * @return: true 成功获取解析成功的数据，false 代表数据获取失败
 */
bool DataParser::PopFrame(uint8_t *data, int &length) {
    std::lock_guard<std::recursive_mutex> lk(mutex1);
    if (!SearchHeader()) {
        return false;
    }
    int size = GetFrameSize();
    if (storage.CurrentBufSize() < size) {
        return false;
    }
    if (!check()) {
        storage.PopData();
        SearchHeader();
        return false;
    }
    length = size;
    storage.PopData(data, size);
    return true;
}

ConcreteParser *ConcreteParser::self = new ConcreteParser;
ConcreteParser::ConcreteParser() {
    registry.insert({ "ConcreteParser", this });
    SetHeaderAndTail();
}
int ConcreteParser::GetFrameSize() {
    //基类已经搜索到头部后才调用此函数
    uint8_t ret = GetData(2);
    if (ret == 0x01) {
        return 10;
    } else {
        return 11;
    }
}
void ConcreteParser::SetHeaderAndTail() {
    header[0] = 0xAA;
    header[1] = 0xBB;
    headerLength = 2;
}
bool ConcreteParser::check() {
    //基类已经搜索到头部后才调用此函数
    if (GetData(8) == 0xAA && GetData(9) == 0xBB) {
        return true;
    }
    return false;
}


