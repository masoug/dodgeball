cd src
protoc --cpp_out=./ messages.proto
mv messages.pb.cc messages.pb.cpp
mv messages.pb.h ../inc
cd ..
