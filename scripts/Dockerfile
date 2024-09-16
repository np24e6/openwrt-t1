FROM ubuntu:focal-20240530

# Set environment variables
ENV DEBIAN_FRONTEND noninteractive
ENV LC_ALL C.UTF-8
ENV NODE_MAJOR 20
ENV TZ Etc/UTC

# Set startup command
CMD ["/bin/bash"]

# Install dependencies for C unit testing
RUN apt update && apt install -y \
	gcovr \
	ruby \
	&& gem install ceedling \
	&& rm -rf /var/lib/apt/lists/*

# Install build dependencies
RUN apt update && apt upgrade -y \
	binutils \
	binutils-gold \
	bison \
	build-essential \
	bzip2 \
	ca-certificates \
	curl \
	cmake \
	default-jdk \
	device-tree-compiler \
	devscripts \
	ecj \
	file \
	flex \
	fuse \
	g++ \
	gawk \
	gcc \
	gengetopt \
	gettext \
	git \
	gnupg \
	groff \
	gperf \
	help2man \
	java-wrappers \
	java-propose-classpath \
	jq \
	libc6-dev \
	libffi-dev \
	libexpat-dev \
	libncurses5-dev \
	libpcre3-dev \
	libsqlite3-dev \
	libssl-dev \
	libxml-parser-perl \
	lz4 \
	make \
	ocaml \
	ocaml-findlib \
	ocaml-nox \
	patch \
	pkg-config \
	psmisc \
	python-is-python3 \
	python3.9 \
	python3.9-dev \
	python3.9-distutils \
	python3-yaml \
	rsync \
	sharutils \
	subversion \
	swig \
	u-boot-tools \
	unzip \
	uuid-dev \
	vim-common \
	wget \
	zip \
	zlib1g-dev \
	&& rm -rf /var/lib/apt/lists/*

RUN curl -L -o- https://deb.nodesource.com/setup_"$NODE_MAJOR".x | bash - \
	&& apt update && apt install -y nodejs \
	&& rm -rf /var/lib/apt/lists/*
