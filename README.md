# 2022spring_SKKUNetworkProject_team9
The purpose of this repository is to implement Youtube Streaming Application using ns-3 simulator

## Prepare

### Docker image
```sh
sudo docker pull jaepang/ns-3:team9
```

## Run

### Attach Container
```sh
docker run -it -v /path/to/repository/2022spring_SKKUNetworkProject_team9/scratch:/root/ns-allinone-3.29/ns-3.29/scratch team9/ns-3:base bash
```

### Run Test
```sh
cd /root/ns-allinone-3.29/ns-3.29
./waf --run "p2p-test"
# or
./ waf --run "wifi-test"
```
