FROM rocker/r-base
MAINTAINER Neal Fultz <nfultz@gmail.com>

ENV GRPC_RELEASE_TAG v1.4.5
ENV PROTOC_RELEASE_TAG v3.2.0
ENV R_BASE_VERSION 3.4.2

## install build tools
RUN apt-get update && apt-get install -y \
  build-essential autoconf libtool \
  libgflags-dev libgtest-dev clang libc++-dev \
  unzip git curl wget \
  pkg-config locales  \
  libssl-dev libcurl4-openssl-dev libxml2-dev libfftw3-dev && \
  apt-get clean && rm -rf /var/lib/apt/lists/

## Install grpc and protobuf from deb
RUN apt-get update && apt-get install -y \
  libprotoc-dev libprotobuf-dev protobuf-compiler \
  libgrpc3 libgrpc-dev && \
  apt-get clean && rm -rf /var/lib/apt/lists/



## set locale for R
RUN echo "en_US.UTF-8 UTF-8" >> /etc/locale.gen \
	&& locale-gen en_US.utf8 \
	&& /usr/sbin/update-locale LANG=en_US.UTF-8
ENV LC_ALL en_US.UTF-8
ENV LANG en_US.UTF-8

## install R packages
RUN echo 'options(repos = c(CRAN = "https://cran.rstudio.com/"), download.file.method = "libcurl")' >> /etc/R/Rprofile.site && \
  Rscript -e "install.packages(c('devtools', 'testthat', 'processx', 'RProtoBuf'))" && \
  Rscript -e "devtools::install_github('nfultz/grpc')" && \
  rm -rf /tmp/downloaded_packages/ /tmp/*.rds && \
  rm -rf /var/lib/apt/lists/*
