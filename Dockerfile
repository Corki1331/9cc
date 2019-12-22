
FROM centos:centos7

RUN mkdir -p /work/9cc
WORKDIR /work/9cc
RUN yum install -y gcc git python3 vim make pip

