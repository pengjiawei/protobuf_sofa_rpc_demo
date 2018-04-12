//
// Created by root on 18-4-12.
//
#include <sofa/pbrpc/pbrpc.h>
#include <sofa/pbrpc/mock_test_helper.h>
#include <gtest/gtest.h>
#include <service.pb.h>
using namespace ::sofa::pbrpc;
void MockEchoSuccess(::google::protobuf::RpcController* /*controller*/,
                     const ::google::protobuf::Message* request,
                     ::google::protobuf::Message* response,
                     ::google::protobuf::Closure* done)
{
    SLOG(NOTICE, "MockEchoSuccess() called");
   if ( ((FooRequest*)request)->code() == 0) {
       ((FooResponse *) response)->set_text("mock return code 0 result");
   }
    done->Run();
}

void MockEchoFail(::google::protobuf::RpcController* controller,
                  const ::google::protobuf::Message* /*request*/,
                  ::google::protobuf::Message* /*response*/,
                  ::google::protobuf::Closure* done)
{
    SLOG(NOTICE, "MockEchoFail() called");
    controller->SetFailed("mock failed");
    done->Run();
}

class MockTest : public ::testing::Test
{
protected:
    MockTest() {}
    virtual ~MockTest() {}
    virtual void SetUp() {
        SOFA_PBRPC_SET_LOG_LEVEL(NOTICE);

        // enable mock and register mock method
        SOFA_PBRPC_ENABLE_MOCK();

        _rpc_client = new RpcClient();
    }
    virtual void TearDown() {
        delete _rpc_client;
    }

    RpcClient* _rpc_client;
};

TEST_F(MockTest, mock_test_success)
{
    SOFA_PBRPC_CLEAR_MOCK_METHOD();
    //use macro to register mock method.like a callback
    SOFA_PBRPC_REGISTER_MOCK_METHOD("FooService.Foo",
                                    NewPermanentExtClosure(&MockEchoSuccess));

    //use a new channel
    SLOG(NOTICE,"address prefix :%s",SOFA_PBRPC_MOCK_CHANNEL_ADDRESS_PREFIX);
    RpcChannel mock_channel(_rpc_client, SOFA_PBRPC_MOCK_CHANNEL_ADDRESS_PREFIX + std::string("a:0"));
    FooService_Stub* stub = new FooService_Stub(&mock_channel);
    RpcController* controller = new sofa::pbrpc::RpcController();
    FooRequest* request = new FooRequest();
    request->set_code(0);
    FooResponse* response = new FooResponse();

    stub->Foo(controller, request, response, NULL);

    SLOG(NOTICE, "ErrorText: %s", controller->ErrorText().c_str());
    ASSERT_FALSE(controller->Failed());
    ASSERT_TRUE(controller->IsRequestSent());
    SLOG(NOTICE,"Sent Bytes = %d",controller->SentBytes());
//    ASSERT_GT(controller->SentBytes(), 0);
    printf("response = %s\n",response->text().c_str());

    delete request;
    delete response;
    delete controller;
    delete stub;
}
int main(int argc, char **argv){
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}