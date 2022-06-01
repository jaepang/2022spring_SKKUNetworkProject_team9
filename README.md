# 2022spring_SKKUNetworkProject_team9
The purpose of this repository is to implement Youtube Streaming Application using ns-3 simulator.BY adding buffer to bridge node, we could reduce traffic congestion on streamer and bridge connection link. 

## Prepare
First, copy some files under below to specified path.
```sh
cp scratch/bridge/model/buf_bridge-net-device.cc ../src/bridge/model/buf_bridge-net-device.cc
cp scratch/brdige/model/buf_bridge-net-device.h ../src/bridge/model/buf_bridge-net-device.h
cp scratch/bridge/helper/buf_bridge-helper.cc ../src/bridge/helper/buf_bridge-helper.cc
cp scratch/bridge/helper/buf_bridge-helper.h ../src/bridge/helper/buf_bridge-helper.h
```
```sh
cp scratch/model/streamer-client.cc ../src/applications/model/streamer-client.cc
cp scratch/model/streamer-client.h ../src/applications/model/streamer-client.h
cp scratch/model/streamer-server.cc ../src/applications/model/streamer-server.cc
cp scratch/model/streamer-server.h ../src/applications/model/streamer-server.h
```
```sh
cp scratch/helper/streamer-client-server-helper.cc ../src/applications/helper/streamer-client-server-helper.cc
cp scratch/helper/streamer-client-server-helper.h ../src/applications/helper/streamer-client-server-helper.h
```
```sh
cp scratch/internet/model/buf-header.cc ../src/internet/model/buf-header.cc
cp scratch/internet/model/buf-header.h ../src/internet/model/buf-header.h
```
```sh
cp scratch/testing.cc /scratch/testing.cc
cp scratch/topology1.cc ../scratch/topology1.cc 
```
```sh
cp scratch/bridge/wscript ../src/bridge/wscript
cp scratch/wscript ..src/applications/wscript
cp scratch/internet/wscript ../src/internet/wscript
```



### Docker image
You can easily excecute by pulling docker image to your local.
```sh
sudo docker pull yunminjin2/ns-3:team9
```

### Attach Container
```sh
docker run -it -v /path/to/repository/2022spring_SKKUNetworkProject_team9/scratch:/root/ns-allinone-3.29/ns-3.29/scratch team9/ns-3:base bash
```

### Run Test
```sh
./waf --run <file-path> 
```
Example:
```sh
./waf --run testing 
```
or 
```sh
./waf --run topology1
```
