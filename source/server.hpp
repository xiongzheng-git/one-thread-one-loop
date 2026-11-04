#include<vector>
#include<cstdint>
#include<algorithm>
#include<cassert>
#include<string>
#include<iostream>
#define Buffer_Default_Size 1024
class buffer{
private:
    std::vector<char> _buffer;//存放数据的buffer
    uint64_t _writer_idx;//读偏移
    uint64_t _reader_idx;//写偏移
public:
    buffer():_writer_idx(0),_reader_idx(0),_buffer(Buffer_Default_Size){}
    char* BeginPos(){
        return &*_buffer.begin();
    }
    //获取读位置
    char* ReadPos(){
        return BeginPos()+_reader_idx;
    }
    //获取写位置
    char* WritePos(){
        return BeginPos()+_writer_idx;
    }
    //确保写空间足够
    void EnsureSpaceEnough(uint64_t size){
        if(TailSpaceSize()>size){
            return;
        }
        else if(TailSpaceSize()+HeadSpaceSize()>=size){
            std::copy(ReadPos(),WritePos(),BeginPos());
            _writer_idx=ReadableSize();
            _reader_idx=0;
            return;
        }
        else{//扩容
            _buffer.resize(_writer_idx+size);
        }
    }
    //获取前沿空闲空间大小
    uint64_t TailSpaceSize(){
        return _buffer.size()-_writer_idx;
    }
    //获取后沿空闲空间大小
    uint64_t HeadSpaceSize(){
        return _reader_idx;
    }
    //获取可读数据大小
    uint64_t ReadableSize(){
        return _writer_idx-_reader_idx;
    }
    //移动读偏移
    void MoveReaderIndex(uint64_t size){
        assert(size<=ReadableSize());
        _reader_idx+=size;
    }
    //移动写偏移
    void MoveWriterIndex(uint64_t size){
        assert(size<=TailSpaceSize());
        _writer_idx+=size;
    }
    //读
    void Read(void* buffer,uint64_t size){
        assert(size<=ReadableSize());
        std::copy(ReadPos(),ReadPos()+size,(char*)buffer);
       //std::cout<<(char*)buffer<<std::endl;

    }
    void ReadAndPop(void* buffer,uint64_t size){
        Read(buffer,size);
        MoveReaderIndex(size);
    }
    void ReadAsString(std::string& str,uint64_t size){
        assert(size<=ReadableSize());
        str.resize(size);
        Read(&str[0],size);
        //std::cout<<str<<std::endl;
    }
    void ReadAsStringAndPop(std::string& str,uint64_t size)
    {
        ReadAsString(str,size);
        MoveReaderIndex(size);
    }
    char* FindCRLF(){
        return std::find(ReadPos(),WritePos(),'\n');
    }
    std::string ReadLine(){//读一行数据
        char* pos =FindCRLF();
        if(pos==WritePos()){
            return "";
        }
        std::string str;
        int size =pos-ReadPos();
        // str.resize(size+1);
        // Read(&str[0],size+1);
        ReadAsString(str,size+1);
        return str;
    }
    std::string ReadLineAndPop(){
        std::string str=ReadLine();
        MoveReaderIndex(str.size());
        return str;
    }
    //写
    void Write(const void* buffer,uint64_t size){
        EnsureSpaceEnough(size);
        char* b =(char*) buffer;
        std::copy(b,b+size,WritePos());
    }
    void WriteAndPush(const void* buffer,uint64_t size){
        Write(buffer,size);
        MoveWriterIndex(size);
    }
    void WriteStringToBuffer(const std::string& str){
        Write(str.c_str(),str.size());
    }
    void WriteStringAndPush(const std::string& str){
        WriteStringToBuffer(str);
        MoveWriterIndex(str.size());
    }
    void WriteBufferToBuffer(buffer& buf){
        Write(buf.ReadPos(),buf.ReadableSize());
    }
    void WriteBufferAndPush(buffer& buf){
        WriteBufferToBuffer(buf);
        MoveWriterIndex(buf.ReadableSize());
    }
    //清理缓冲区
    void Clear(){
        _writer_idx=0;
        _reader_idx=0;
    }
};