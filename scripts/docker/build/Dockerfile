# Build environment
FROM debian:bookworm

ARG user
ARG uid
ARG gid
ARG docker_gid
ENV USER=${user}
ENV UID=${uid}
ENV GID=${gid}
ENV DOCKER_GID=${docker_gid}

RUN if [ "$DOCKER_GID" != "${GID}" ] ; then groupadd --gid ${DOCKER_GID} docker ; fi

RUN if [ ! -d /home/${USER} ] ; then mkdir -p /home/${USER} && chmod -R 777 /home/${USER} ; fi

# Install basic dependencies & useful tools. libosip2/libeXosip2 are
# not in Ubuntu apt, so they are built from source below; here are the
# build prerequisites (autotools, pkg-config, OpenSSL for TLS transport).
RUN apt-get update && \
    apt-get install -y \
        autoconf \
        automake \
        bison \
        build-essential \
        ccache \
        cmake \
        curl \
        docker.io \
        file \
        flex \
        gawk \
        libbsd-dev \
        libjansson-dev \
        libexpat1 \
        libexpat1-dev \
        libcurl4-openssl-dev \
        libglib2.0-dev \
        libnetconf2-dev \
        libncurses5 \
        libncurses5-dev \
        libpam0g \
        libpam0g-dev \
        libpcap0.8-dev \
        libpopt-dev \
        libreadline-dev \
        libssh-dev \
        libssh2-1-dev \
        libssl-dev \
        libxml2 \
        libxml2-dev \
        libxslt1-dev \
        libxslt1.1 \
        libpcre3-dev \
        libyaml-dev \
        m4 \
        meson \
        openssh-server \
        pkg-config \
        perl \
        python3 \
        rsync \
        ssh \
        sudo \
        tshark \
        tcl-dev \
        unzip \
        wget

RUN echo "${USER} ALL=NOPASSWD: ALL" >> /etc/sudoers

RUN if [ "$DOCKER_GID" != "${GID}" ] ; then groupadd --gid ${GID} ${USER} ; fi

RUN useradd --system \
            --gid ${GID} --gid ${DOCKER_GID} \
            --uid ${UID} \
            --shell /bin/bash \
            ${USER}
RUN usermod -a -G ${DOCKER_GID} ${USER}
RUN usermod -a -G docker ${USER}

# libosip2 + libeXosip2 from source: neither ships a -dev package in
# Ubuntu apt, and ta_sip links both. Pin known-good releases; adjust
# the versions on a first real build if the mirror moves them.
ARG OSIP_VERSION=5.3.0
ARG EXOSIP_VERSION=5.3.0
RUN set -eux; \
    cd /tmp; \
    curl -fsSLO "https://download.savannah.gnu.org/releases/osip/libosip2-${OSIP_VERSION}.tar.gz"; \
    tar xf "libosip2-${OSIP_VERSION}.tar.gz"; \
    cd "libosip2-${OSIP_VERSION}"; ./configure --prefix=/usr/local; make -j"$(nproc)"; make install; \
    cd /tmp; \
    curl -fsSLO "https://download.savannah.gnu.org/releases/exosip/libexosip2-${EXOSIP_VERSION}.tar.gz"; \
    tar xf "libexosip2-${EXOSIP_VERSION}.tar.gz"; \
    cd "libexosip2-${EXOSIP_VERSION}"; ./configure --prefix=/usr/local; make -j"$(nproc)"; make install; \
    ldconfig; \
    rm -rf /tmp/libosip2-* /tmp/libexosip2-*
