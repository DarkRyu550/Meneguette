
CLIENT_SOURCES=UnixExceptions.cc Endian.cc Net.cc Client.cc
Client: $(CLIENT_SOURCES)
	$(CXX) $(CXXFLAGS) -o $@ $(CLIENT_SOURCES)

SERVER_SOURCES=UnixExceptions.cc Endian.cc Net.cc Server.cc
Server: $(SERVER_SOURCES)
	$(CXX) $(CXXFLAGS) -o $@ $(SERVER_SOURCES)

all: Client Server