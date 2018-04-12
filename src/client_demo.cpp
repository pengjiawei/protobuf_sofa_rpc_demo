#include <iostream>
#include <sofa/pbrpc/pbrpc.h>
#include "service.pb.h"
#include <unistd.h>
using namespace google::protobuf;

void Done(sofa::pbrpc::RpcController* controller,
          FooRequest* request,
          FooResponse* response,
          bool* callbacked){
    SLOG(NOTICE, "RemoteAddress=%s", controller->RemoteAddress().c_str());
    SLOG(NOTICE, "IsRequestSent=%s", controller->IsRequestSent() ? "true" : "false");
    if (controller->IsRequestSent())
    {
        SLOG(NOTICE, "LocalAddress=%s", controller->LocalAddress().c_str());
        SLOG(NOTICE, "SentBytes=%ld", controller->SentBytes());
    }

    if (controller->Failed()) {
        SLOG(ERROR, "request failed: %s", controller->ErrorText().c_str());
    }
    else {
        SLOG(NOTICE, "request succeed: %s", response->text().c_str());
    }

    delete controller;
    delete request;
    delete response;
    *callbacked = true;
    printf("rpc done\n");
}
int main() {
    SOFA_PBRPC_SET_LOG_LEVEL(NOTICE);
    //define a client
    sofa::pbrpc::RpcClientOptions clientOptions;
    sofa::pbrpc::RpcClient rpcClient(clientOptions);
    //define a channel
    sofa::pbrpc::RpcChannelOptions channelOptions;
    sofa::pbrpc::RpcChannel rpc_channel(&rpcClient,"127.0.0.1:8421",channelOptions);
    //controller
    sofa::pbrpc::RpcController* controller = new sofa::pbrpc::RpcController();

    FooRequest* request = new FooRequest();
    request->set_code(2);
    FooResponse* response = new FooResponse();

    //default  sync call
//    FooService_Stub* stub = new FooService_Stub(&rpc_channel);
//    stub->Foo(controller,request,response,NULL);

    //async call,register a call back
    bool callbacked = false;
    Closure* done = sofa::pbrpc::NewClosure(&Done,controller,request,response,&callbacked);
    FooService_Stub* stub = new FooService_Stub(&rpc_channel);
    //add a closure in last param
    stub->Foo(controller,request,response,done);
    printf("failed = %d\n",controller->Failed());
    if(controller->Failed()){
        printf("fail %s\n",controller->ErrorText().c_str());
    }else {
        printf("success %s\n",response->text().c_str());
    }
    //if async ,hold on
    while (!callbacked){
        sleep(1);
    }
    delete controller;
    delete request;
    delete response;
    delete  stub;
    return 0;
}