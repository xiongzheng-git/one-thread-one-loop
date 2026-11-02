#include<unordered_map>
#include<memory>
#include<cstdint>
#include<functional>
#include<vector>
#include<iostream>
#include<unistd.h>

using TaskFunc =std::function<void()>;
using ReleaseFuc =std::function<void()>;
class TimerTask{
    uint64_t _id;//任务唯一标识
    uint32_t _timeout;//超时时间
    TaskFunc _cb;//任务回调函数
    ReleaseFuc _release_cb;
public:
    TimerTask(uint64_t id,uint32_t timeout,const TaskFunc& cb)
    :_id(id),_timeout(timeout),_cb(cb){}
    ~TimerTask(){
        _cb();
        _release_cb();
    }
    void set_release_cb(const ReleaseFuc& cb){
        _release_cb=cb;
    }
    uint32_t get_timeout(){
        return _timeout;
    }
};
class TimerWheel{
    using PtrTask=std::shared_ptr<TimerTask>;
    using WeakPtr =std::weak_ptr<TimerTask>;
    int _tick;//滴答指针
    int _capacity;
    std::vector<std::vector<PtrTask>> _wheel;//时间轮
    std::unordered_map<uint64_t,WeakPtr> _id_ptr_map;
    void remove_map(uint64_t id){
        _id_ptr_map.erase(id);
    }
public:
    TimerWheel()
    :_capacity(60),_wheel(_capacity),_tick(0)
    {}
    void AddTask(uint64_t id,uint32_t timeout,const TaskFunc& cb){
        PtrTask pt(new TimerTask(id,timeout,cb));
        pt->set_release_cb(std::bind(&TimerWheel::remove_map,this,id));
        _wheel[(_tick+timeout)%_capacity].push_back(pt);
        WeakPtr weak(pt);
        _id_ptr_map[id] =weak;
    }
    void RefreshTask(uint64_t id){
        auto it =_id_ptr_map.find(id);
        if(it==_id_ptr_map.end()){
            return;
        }
        WeakPtr weak =it->second;
        PtrTask pt =weak.lock();
        uint32_t timeout =pt->get_timeout();
        _wheel[(_tick+timeout)%_capacity].push_back(pt);
    }
    void Run(){
        _tick=(_tick+1)%_capacity;
        _wheel[_tick].clear();
    }
};
class test{
public:
    test(){std::cout<<"构造"<<std::endl;}
    ~test(){std::cout<<"析构"<<std::endl;}
};
void Delete(test* t){
    if(t) delete t;
}
int main()
{
    TimerWheel wheel;
    test* t =new test;
    wheel.AddTask(111,5,std::bind(Delete,t));
    for(int i=0;i<5;i++){
        sleep(1);
        wheel.RefreshTask(111);
        std::cout<<"延迟任务"<<std::endl;
        wheel.Run();
    }
    while(1){
        sleep(1);
        std::cout<<"-----------"<<std::endl;
        wheel.Run();
    }
    return 0;
}