#include <iostream>
#include <sofa/pbrpc/pbrpc.h>
#include "service.pb.h"
using namespace google::protobuf;

void Done(){
    printf("rpc done\n");
}
int main() {
    //define a client
    sofa::pbrpc::RpcClientOptions clientOptions;
    sofa::pbrpc::RpcClient rpcClient(clientOptions);
    //define a channel
    sofa::pbrpc::RpcChannelOptions channelOptions;
    sofa::pbrpc::RpcChannel rpc_channel(&rpcClient,"127.0.0.1:8421",channelOptions);
    //controller
    sofa::pbrpc::RpcController* controller = new sofa::pbrpc::RpcController();

    FooRequest* request = new FooRequest();
    request->set_code(0);
    FooResponse* response = new FooResponse();

    //default  sync call
    FooService::Stub* stub = new FooService::Stub(&rpc_channel);
    stub->Foo(controller,request,response,NULL);
    if(controller->Failed()){
        printf("fail %s\n",controller->ErrorText().c_str());
    }else {
        printf("success %s\n",response->text().c_str());
    }
    delete controller;
    delete request;
    delete response;
    delete  stub;
    return 0;
}