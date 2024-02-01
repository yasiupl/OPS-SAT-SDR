#!/usr/bin/env bash

commands=$1

BRANCH=master
IMAGE=gitlab.com/esa/nmf/ops-sat-sepp-ci-docker
WORKDIR=$(dirname $0)/../
CONTAINER=sepp_build

installed=`docker image ls | grep $IMAGE`
if [ -z "$installed" ]; then
    echo SEPP CI Docker not present, installing...
    #docker build -t $CONTAINER ./ops-sat-sepp-ci-docker 
    docker run -it --entrypoint /bin/bash --name $CONTAINER -u 0 -v $WORKDIR:/home/user/share $CONTAINER
    installed=`docker image ls | grep $IMAGE`
fi
echo Found Image: $installed

running=`docker ps | grep $CONTAINER`
if [ -z "$running" ]; then
    echo SEPP CI Docker not started, starting...
    docker start $CONTAINER
    running=`docker ps | grep $CONTAINER`
fi
echo Found container: $running

echo SEPP CI Docker started, entering bash shell...
docker exec -it $CONTAINER bash -c "sudo su - user; source poky_sdk/environment-setup-cortexa8hf-neon-poky-linux-gnueabi; cd share; export LC_ALL=$(locale -a|grep -ix 'c.utf-\?8' || echo C); bash -c \'$commands\'; bash"