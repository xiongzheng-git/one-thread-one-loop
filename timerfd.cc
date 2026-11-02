#include<iostream>
#include<sys/timerfd.h>
#include<ctime>
#include<unistd.h>
int main(){
    //创建定时器
    int timerfd = timerfd_create(CLOCK_MONOTONIC,0);
    itimerspec spec;
    spec.it_value.tv_sec=3;
    spec.it_value.tv_nsec=0;
    spec.it_interval.tv_sec=3;
    spec.it_interval.tv_nsec=0;
    //启动定时器
    timerfd_settime(timerfd,0,&spec,nullptr);
    time_t begin =time(nullptr);
    uint64_t count=0;
    while(1){
        int ret=read(timerfd,&count,sizeof(count));
        if(ret<0){
            return -1;
        }
        std::cout<<count<<" "<<time(nullptr)-begin<<std::endl;
    }
    close(timerfd);
    return 0;
}