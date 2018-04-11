//
// Created by root on 18-4-11.
//
#include <string>
#include <unistd.h>
#include <sofa/pbrpc/pbrpc.h>
#include "service.pb.h"
void echo(std::string user){
    printf("hello + %s\n",user.c_str());
}
class FooServiceImpl : public FooService{
public:
    FooServiceImpl() {}

    virtual ~FooServiceImpl() {

    }

    virtual void Foo(::google::protobuf::RpcController *controller, const ::FooRequest *request, ::FooResponse *response,
             ::google::protobuf::Closure *done) override {
        //controller can return some basic message of the request process,such as reason
        if (request->code() == 0){
            response->set_text("I know you are code 0");
        } else if(request->code() == 1){
            response->set_text("I know you are code 1");
        } else {
            response->set_text("default");
        }
        done->Run();
    }
};

bool thread_init_func()
{
    sleep(1);
    printf("Init work thread succeed");
    return true;
}

void thread_dest_func()
{
    printf("Destroy work thread succeed");
}

int main(){
    sofa::pbrpc::RpcServerOptions options;
    options.work_thread_init_func = sofa::pbrpc::NewPermanentExtClosure(&thread_init_func);
    options.work_thread_dest_func = sofa::pbrpc::NewPermanentExtClosure(&thread_dest_func);
    sofa::pbrpc::RpcServer rpc_server(options);
    std::string address = "0.0.0.0:8421";
    bool flag = rpc_server.Start(address);
    if(!flag)
        return -1;
    FooService* fooService = new FooServiceImpl();
    if(!rpc_server.RegisterService(fooService)){
        printf("register service failed\n");
    }
//    while(1){
//        sleep(1);
//    }
    rpc_server.Run();
//    delete options.work_thread_init_func;
//    delete options.work_thread_dest_func;
}