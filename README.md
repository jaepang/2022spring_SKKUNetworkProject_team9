# 2022spring_SKKUNetworkProject_team9
The purpose of this repository is to implement Youtube Streaming Application using ns-3 simulator.BY adding buffer to bridge node, we could reduce traffic congestion on streamer and bridge connection link. 

## Prepare
First, copy some files under below to specified path.

  * scratch/bridge/model/buf_bridge-net-device.cc, buf_bridge-net-device.h  -->  ns-3.29/src/bridge/model/buf_bridge-net-device.cc, buf_bridge-net-device.h
  * scratch/bridge/helper/buf_bridge-helper.cc, buf_bridge-helper.h -->  ns-3.29/src/bridge/helper/buf_bridge-helper.cc, buf_bridge-helper.h

  * scratch/model/streamer-client.cc, streamer-client.h, streamer-server.cc streamer-server.h --> ns-3.29/src/applications/model/streamer-client.cc, streamer-client.h, streamer-server.cc streamer-server.h
  * scratch/helper/streamer-client-server-helper.cc, streamer-client-server-helper.h --> ns-3.29/src/applications/helper/streamer-client-server-helper.cc, streamer-client-server-helper.h

  * scratch/internet/model/buf-header.cc buf-header.h --> ns-3.29/src/internet/model/buf-header.cc, buf-header.h

  * scratch/testing.cc, topology1.cc --> ns-3.29/scratch/testing.cc, topology1.cc

  * scratch/bridge/wscript  --> ns-3.29/src/bridge/wscript
  * scratch/wscript --> ns-3.29/src/applications/wscript
  * scratch/internet/wscript --> ns-3.29/src/internet/wscript

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
