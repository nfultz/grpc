FROM ubuntu:latest
MAINTAINER Gergely Daroczi <gergely@system1.com>

ENV GRPC_RELEASE_TAG v1.4.5
ENV PROTOC_RELEASE_TAG v3.2.0
ENV R_BASE_VERSION 3.4.2

## install build tools
RUN apt-get update && apt-get install -y \
  build-essential autoconf libtool \
  libgflags-dev libgtest-dev clang libc++-dev \
  unzip git curl wget \
  pkg-config locales && \
  apt-get clean && rm -rf /var/lib/apt/lists/

## build grpc and protobuf
RUN git clone -b ${GRPC_RELEASE_TAG} https://github.com/grpc/grpc /var/local/git/grpc && \
  cd /var/local/git/grpc && git submodule update --init && \
  cd /var/local/git/grpc/third_party/protobuf && \
  git checkout ${PROTOC_RELEASE_TAG} && \
  ./autogen.sh && ./configure && \
  make && make install && make clean && ldconfig && \
  cd /var/local/git/grpc && \
  make && make install && make clean && ldconfig

## set locale for R
RUN echo "en_US.UTF-8 UTF-8" >> /etc/locale.gen \
	&& locale-gen en_US.utf8 \
	&& /usr/sbin/update-locale LANG=en_US.UTF-8
ENV LC_ALL en_US.UTF-8
ENV LANG en_US.UTF-8

## install R
RUN echo "deb http://cloud.r-project.org/bin/linux/ubuntu trusty/" >> /etc/apt/sources.list && \
  apt-key adv --keyserver keyserver.ubuntu.com --recv-keys E084DAB9 && \
  apt-get -qq update && apt-get upgrade -y  && \
  apt-get install -y --no-install-recommends \
  littler \
  r-base-core=${R_BASE_VERSION}* \
  r-base-dev=${R_BASE_VERSION}* \
  r-recommended=${R_BASE_VERSION}* \
  libssl-dev libcurl4-openssl-dev libxml2-dev libfftw3-dev && \
  apt-get clean && rm -rf /var/lib/apt/lists/

## install R packages
RUN echo 'options(repos = c(CRAN = "https://cran.rstudio.com/"), download.file.method = "libcurl")' >> /etc/R/Rprofile.site && \
  echo 'source("/etc/R/Rprofile.site")' >> /etc/littler.r && \
  Rscript -e "install.packages('docopt')" && \
  ln -s /usr/share/doc/littler/examples/install2.r /usr/local/bin/install2.r && \
  ln -s /usr/share/doc/littler/examples/installGithub.r /usr/local/bin/installGithub.r && \
  install2.r --error remotes devtools RProtoBuf && \
  installGithub.r nfultz/grpc && \
  rm -rf /tmp/downloaded_packages/ /tmp/*.rds && \
  rm -rf /var/lib/apt/lists/*
