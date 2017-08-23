# Compile **xmr-stak** for Linux

### GNU Compiler
```
    # Ubuntu / Debian
    sudo apt install libmicrohttpd-dev libssl-dev cmake build-essential libhwloc-dev
    git clone https://github.com/fireice-uk/xmr-stak-cpu.git
    cd xmr-stak-cpu
    cmake .
    make install

    # Arch
    sudo pacman -S base-devel hwloc openssl cmake libmicrohttpd
    cmake .
    make install

    # Fedora
    sudo dnf install gcc gcc-c++ hwloc-devel libmicrohttpd-devel openssl-devel cmake
    cmake .
    make install

    # CentOS
    sudo yum install centos-release-scl cmake3 hwloc-devel libmicrohttpd-devel openssl-devel
    sudo yum install devtoolset-4-gcc*
    sudo scl enable devtoolset-4 bash
    cmake3 .
    make install

    # Ubuntu 14.04
    sudo add-apt-repository ppa:ubuntu-toolchain-r/test
    sudo apt update
    sudo apt install gcc-5 g++-5 make
    sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-5 1 --slave /usr/bin/g++ g++ /usr/bin/g++-5
    curl -L http://www.cmake.org/files/v3.4/cmake-3.4.1.tar.gz | tar -xvzf - -C /tmp/
    cd /tmp/cmake-3.4.1/ && ./configure && make && sudo make install && cd -
    sudo update-alternatives --install /usr/bin/cmake cmake /usr/local/bin/cmake 1 --force
    sudo apt install libmicrohttpd-dev libssl-dev libhwloc-dev
    cmake .
    make install
```

- g++ version 5.1 or higher is required for full C++11 support. CMake release compile scripts, as well as CodeBlocks build environment for debug builds is included.

### To do a static build for a system without gcc 5.1+
```
    cmake -DCMAKE_LINK_STATIC=ON .
    make install
```
Note - cmake caches variables, so if you want to do a dynamic build later you need to specify '-DCMAKE_LINK_STATIC=OFF'



