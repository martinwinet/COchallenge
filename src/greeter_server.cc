#include <iostream>
#include <memory>
#include <string>

#include "sample.grpc.pb.h" // communication protocol
#include <grpc++/grpc++.h>  // gRCP
#include "../json/json.hpp" // JSON
#include <pqxx/pqxx>        // interface to PostgreSQL

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using cabonlinechallenge::CabonlineChallenge;
using cabonlinechallenge::SetChallengeDataRequest;
using cabonlinechallenge::SetChallengeDataReply;


// server logic
class GreeterServiceImpl final : public CabonlineChallenge::Service 
{
  Status SetChallengeData(ServerContext*                 context, 
                          const SetChallengeDataRequest* request,
                          SetChallengeDataReply*         reply)  
  override {
  
    // connect to database
    pqxx::connection C("dbname = challengeDB " 
                       "user = postgres " 
                       "password = Ne0wvb12 " 
                       "hostaddr = 127.0.0.1 " 
                       "port = 5432");
    
    // deserialize request from client to json
    nlohmann::json deserRequest = nlohmann::json::parse(request->requestcontent());

    // handle requests
    if (deserRequest["msgFlag"]==1) 
    { 
      try
      {
        // create SQL statement
        std::string newDriver = deserRequest["dbEntry"]["driver"];
        std::string newCar    = deserRequest["dbEntry"]["car"];
        std::string sqlInsert = "INSERT INTO challengetable (driver,car) "
                                "VALUES ('" + newDriver + "','" + newCar +"');";
       
        // execute SQL statement
        pqxx::work W(C);
        W.exec(sqlInsert);
        W.commit();

        // send response to client
        reply->set_responsecontent("Database updated.");
      }
      catch(const std::exception &e)
      {
        // send error to client
        reply->set_responsecontent("Database update failed :-(");
      }

    } 
    else if (deserRequest["msgFlag"]==2) 
    {
      try
      {
        // create SQL statement
        std::string sqlSelect = "SELECT * from challengetable;";

        // execute SQL statement
        pqxx::nontransaction N(C);
        pqxx::result R(N.exec(sqlSelect));

        // assemble reply as JSON message
        nlohmann::json message;
        for (pqxx::result::const_iterator c = R.begin(); c != R.end(); ++c) 
        {
          std::string key = "driver " + c[0].as<std::string>();
          message[key] = {{"driver", c[1].as<std::string>()},
                          {"car",    c[2].as<std::string>()}};
        }

        // serialize
        std::string messageString = message.dump();

        // send response to client
        reply->set_responsecontent(messageString);
      }
      catch(const std::exception &e)
      {
        // send error to client
        reply->set_responsecontent("Database query failed :-("); 
      }
    } 
    else
    {
      reply->set_responsecontent("Sorry, invalid message flag :-(");
    }
    
    // disconnect from database
    C.disconnect ();

    // return status
    return Status::OK;
  }
};


void RunServer() {

  // server setup
  std::string server_address("0.0.0.0:50051");
  GreeterServiceImpl service;
  ServerBuilder builder;
  
  // listen on the given address, no authentication mechanism.
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());

  // Register "service" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *synchronous* service.
  builder.RegisterService(&service);

  // assemble the server.
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;

  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  server->Wait();
}


int main(int argc, char** argv) {
  
  RunServer();

  return 0;
}
