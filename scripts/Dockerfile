FROM ubuntu:focal

RUN apt-get update && DEBIAN_FRONTEND=noninteractive TZ=Etc/UTC \
	apt-get install -y \
	binutils \
	binutils-gold \
	bison \
	build-essential \
	bzip2 \
	curl \
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
	gcc-multilib \
	gengetopt \
	gettext \
	git \
	groff \
	help2man \
	java-wrappers \
	java-propose-classpath \
	jq \
	libc6-dev \
	libncurses5-dev \
	libpcre3-dev \
	libssl-dev \
	libxml-parser-perl \
	lz4 \
	make \
	ocaml \
	ocaml-findlib \
	ocaml-nox \
	patch \
	pkg-config \
	python-is-python3 \
	python-yaml \
	python2.7 \
	python3 \
	python3-dev \
	python3-distutils \
	python3-yaml \
	rsync \
	ruby \
	sharutils \
	subversion \
	swig \
	u-boot-tools \
	unzip \
	vim-common \
	wget \
	zip \
	zlib1g-dev

# install NodeJS v14
RUN curl -sL https://deb.nodesource.com/gpgkey/nodesource.gpg.key | apt-key add - && \
	/bin/echo 'deb https://deb.nodesource.com/node_14.x focal main' > /etc/apt/sources.list.d/nodesource.list && \
	/bin/echo 'deb-src https://deb.nodesource.com/node_14.x focal main' >> /etc/apt/sources.list.d/nodesource.list && \
	apt-get update && apt-get install -y nodejs


CMD ["/bin/bash"]
