FROM ubuntu:focal

ENV NODE_MAJOR 18

RUN apt-get update && DEBIAN_FRONTEND=noninteractive TZ=Etc/UTC \
	apt-get install -y \
	binutils \
	binutils-gold \
	bison \
	build-essential \
	bzip2 \
	ca-certificates \
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
	gnupg \
	groff \
	help2man \
	java-wrappers \
	java-propose-classpath \
	jq \
	libc6-dev \
	libffi-dev \
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
	uuid-dev \
	vim-common \
	wget \
	zip \
	zlib1g-dev

# install NodeJS v14
RUN mkdir -p /etc/apt/keyrings && \
	curl -fsSL https://deb.nodesource.com/gpgkey/nodesource-repo.gpg.key | gpg --dearmor -o /etc/apt/keyrings/nodesource.gpg && \
	/bin/echo "deb [signed-by=/etc/apt/keyrings/nodesource.gpg] https://deb.nodesource.com/node_$NODE_MAJOR.x nodistro main" | tee /etc/apt/sources.list.d/nodesource.list && \
	apt-get update && apt-get install -y nodejs

CMD ["/bin/bash"]
