//
// Created by root on 18-4-14.
//
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
        sofa::pbrpc::RpcController* sofa_controller = static_cast<sofa::pbrpc::RpcController*>(controller);
        printf("request from %s : request code = %d\n",sofa_controller->RemoteAddress().c_str(), request->code());
        printf("http path = %s\n",sofa_controller->HttpPath().c_str());

        std::map<std::string, std::string>::const_iterator it;
        const std::map<std::string, std::string>& headers = sofa_controller->HttpHeaders();
        for (it = headers.begin(); it != headers.end(); ++it) {
            printf("Header[\"%s\"]=\"%s\"\\\n", it->first.c_str(), it->second.c_str());
        }

        if (request->code() == 0){
            response->set_text("I know you are code 0");
        } else if(request->code() == 1){
            response->set_text("I know you are code 1");
        } else {
            response->set_text("default text");
        }
        done->Run();
    }
};

bool thread_init_func()
{
    sleep(1);
    printf("Init work thread succeed\n");
    return true;
}

void thread_dest_func()
{
    printf("Destroy work thread succeed\n");
}

int main(){
    sofa::pbrpc::RpcServerOptions options;

    options.disable_builtin_services = true;
    options.no_delay = true;
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
    delete options.work_thread_init_func;
    delete options.work_thread_dest_func;
}
