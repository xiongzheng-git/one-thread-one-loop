#include<iostream>
#include<string>
#include"server.hpp"

int main(){

    buffer buf;
    for(int i=0;i<200;i++){
        std::string str ="ni hao"+std::to_string(i)+'\n';
        buf.WriteStringAndPush(str);
    }
    while(buf.ReadableSize()>0){
        std::string str;
        str=buf.ReadLineAndPop();
        std::cout<<str<<std::endl;
    }

    // buf.WriteStringAndPush(str);
    // // std::string tmp;
    // buffer buf1;
    // buf1.WriteBufferAndPush(buf);
    // // std::cout<<str.length()<<std::endl;
    // std::cout<<buf.ReadableSize()<<std::endl;
    // std::cout<<buf1.ReadableSize()<<std::endl;
    // std::string tmp;
    // buf1.ReadAsStringAndPop(tmp,buf1.ReadableSize());
    // std::cout<<tmp<<std::endl;
    // std::cout<<buf1.ReadableSize()<<std::endl;

    return 0;
}