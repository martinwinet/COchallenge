# COchallenge
A simple C++ gRCP service using JSON to query a PostgreSQL database

## 1.) PROTBUF AND GRPC

Install and compile Protobuf and gRCP. 


## 2.) POSTGRESQL

### Install PostgreSQL
l. user@server $ sudo apt-get install postgresql-client
l. user@server $ sudo apt-get install postgresql postgresql-contrib

### Set up password and database
l. user@server $ sudo -u postgres psql postgres        (jump into postgres terminal) 
l. postgres=#  $ \password                             (command to define a new password)
l. postgres=#  $ 1234                                  (set pasword for postgresql)
l. postgres=#  $ \q                                    (exit)
l. postgres@server  $ exit                             (logout)
l. user@server $ sudo -u postgres createdb challengeDB (create a DB with name "challengeDB")
l. user@server $ sudo -i -u postgres                   (switch to postres account)
l. postgres@server $ psql -d challengeDB -U postgres   (jump into challengeDB database)

### Create table and add some data
l. challengeDB=# CREATE TABLE challengeTable (id serial PRIMARY KEY,
                                           driver varchar (20) NOT NULL,
                                           car varchar (10) NOT NULL);
l. challengeDB=# INSERT INTO challengetable (driver, car) VALUES ('martin','volvoXC90');


## 3.) LIBPQXX

Install and compile libpqxx 4.0.2


## 4.) COMPILE PROJECT

l. Edit the file "greeter_server.cc" in the "src" directory and set the correct database password
l. Run "make" in the "src" directory.
l. Open a terminal in the "src" directory and run the RPC server with "./greeter_server
l. Open another terminal in the directory and run the RPC client with "./greeter_client

