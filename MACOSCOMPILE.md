# Compile **xmr-stak** for MacOS

Assuming you already have [Homebrew](https://brew.sh) installed, the compilation is pretty straightforward:

```
brew install hwloc libmicrohttpd gcc openssl cmake
cmake . -DOPENSSL_ROOT_DIR=/usr/local/opt/openssl
make install
```

Both `config.txt` and the `xmr-stak-cpu` binary will be generated in the `bin/` directory.
