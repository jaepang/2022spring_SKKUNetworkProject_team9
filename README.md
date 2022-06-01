# 2022spring_SKKUNetworkProject_team9
The purpose of this repository is to implement Youtube Streaming Application using ns-3 simulator.BY adding buffer to bridge node, we could reduce traffic congestion on streamer and bridge connection link.

## Overview
<img src = "https://user-images.githubusercontent.com/56184924/171410383-c87f1a71-fa8a-47dd-94f8-e2466a224283.png" width="70%" height="70%">

First, the client sends a connection request packet to the server. After that, the packet is delivered to the buffer bridge net device on the csm link. In the buffer bridge net device, the destination address of the packet is first checked to see if it is a buffer already formed in its net device. If the destination streamer is already the first server to be requested, the buffer bridge net device will form a new buffer. The buffer contains a list of m_buffers that store packets and clients that want to be connected in the m_list buffer. Thereafter, the buffer bridge net device sends a packet request to the streaming server.<br/>
When the streaming server receives the request packet, the packet sends its own streaming packet to the on-ip address, where the packet is also forwarded to the buffer bridge net device via csm link. When a buffer bridge net device receives a packet from a registered server, it puts the packet in the m_buffer of the server buffer. Thereafter, the buffer bridge net device delivers the incoming packets to the clients registered in its m_list.<br/>
In this way, we can reduce the traffic congestion of links connected between streaming servers and bridge devices. <br/>

## Prepare
### Option 1 - Manually copy files from repository
First, copy some files under below to specified path. 
```sh
mv scratch/bridge/model/buf_bridge-net-device.cc ../src/bridge/model/buf_bridge-net-device.cc
mv scratch/brdige/model/buf_bridge-net-device.h ../src/bridge/model/buf_bridge-net-device.h
mv scratch/bridge/helper/buf_bridge-helper.cc ../src/bridge/helper/buf_bridge-helper.cc
mv scratch/bridge/helper/buf_bridge-helper.h ../src/bridge/helper/buf_bridge-helper.h
```
```sh
mv scratch/model/streamer-client.cc ../src/applications/model/streamer-client.cc
mv scratch/model/streamer-client.h ../src/applications/model/streamer-client.h
mv scratch/model/streamer-server.cc ../src/applications/model/streamer-server.cc
mv scratch/model/streamer-server.h ../src/applications/model/streamer-server.h
```
```sh
mv scratch/helper/streamer-client-server-helper.cc ../src/applications/helper/streamer-client-server-helper.cc
mv scratch/helper/streamer-client-server-helper.h ../src/applications/helper/streamer-client-server-helper.h
```
```sh
mv scratch/internet/model/buf-header.cc ../src/internet/model/buf-header.cc
mv scratch/internet/model/buf-header.h ../src/internet/model/buf-header.h
```
```sh
mv scratch/testing.cc /scratch/testing.cc
mv scratch/topology1.cc ../scratch/topology1.cc 
```
```sh
mv scratch/bridge/wscript ../src/bridge/wscript
mv scratch/wscript ..src/applications/wscript
mv scratch/internet/wscript ../src/internet/wscript
```

### Option 2 - Pull Docker image
You can easily excecute by pulling docker image to your local.
```sh
sudo docker pull yunminjin2/ns-3:team9
```

#### Run & Attach to Pulled image
```sh
sudo docker run -it -v /path/to/repository/2022spring_SKKUNetworkProject_team9/scratch:/root/ns-allinone-3.29/ns-3.29/scratch team9/ns-3:base bash
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
