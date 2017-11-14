#include <iostream>
#include <memory>
#include <string>

#include "sample.grpc.pb.h" // communication protocol
#include <grpc++/grpc++.h>  // gRCP
#include "../json/json.hpp" // JSON

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using cabonlinechallenge::CabonlineChallenge;
using cabonlinechallenge::SetChallengeDataRequest;
using cabonlinechallenge::SetChallengeDataReply;


class GreeterClient {
 
 public:
  
  // constructor
  GreeterClient(std::shared_ptr<Channel> channel)
      : stub_(CabonlineChallenge::NewStub(channel)) {}

  // assemble and send client's payload, present server reply
  std::string SetChallengeData(const std::string& message) {

    // payload
    SetChallengeDataRequest request;
    request.set_requestcontent(message);

    // prepare reply from server
    SetChallengeDataReply reply;

    // context for the client
    ClientContext context;

    // THE ACTUAL RPC
    Status status = stub_->SetChallengeData(&context, request, &reply);

    // present reply
    if (status.ok()) 
    {
      return reply.responsecontent();
    } 
    else 
    {
      std::cout << status.error_code() << ": " << status.error_message() << std::endl;
      return "RPC failed :-(";
    }
  }

 private:

  std::unique_ptr<CabonlineChallenge::Stub> stub_;

};


int main(int argc, char** argv) {

  // instantiate client
  GreeterClient greeter(grpc::CreateChannel("localhost:50051", 
                                            grpc::InsecureChannelCredentials()));

  // define json message 
  // (could also load it from a .json file)
  nlohmann::json message;
/*  message["msgFlag"] = 2; // 1: new entry for database, 2: request database as JSON, 
  message["dbEntry"] = {{"driver",   "kevin" }, 
                        {"car",      "audiS3"}};
*/

  message["msgFlag"] = 1; // 1: new entry for database, 2: request database as JSON, 
  message["dbEntry"] = {{"driver",   "felix" }, 
                        {"car",      "fabia"}};

  // serialize
  std::string messageString = message.dump();

  // send message to server and get server reply
  std::string reply = greeter.SetChallengeData(messageString);

  // process the server reply
  try
  {
    // deserialize string reply to json
    nlohmann::json deserReply = nlohmann::json::parse(reply);

    // print reply
    std::cout << std::endl << "DESERIALISED REPLY AS JSON" << std::endl;
    std::cout << deserReply.dump(4) << std::endl;
  }
  catch(const std::exception &e)
  {
    // print error message from server
    std::cout << reply << std::endl;
  }

  return 0;
}
