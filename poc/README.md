#### **P**roof **O**f **C**oncept (POC)
This folder shows a proof of concept for using zstandard to compress records in a database on the fly, instead of entire files.
To build the C version of this, you need to install ZSTD. You can also install with multithreading support manually by cloning the repo and running the following:
```
cd zstd
make lib-mt 
sudo make install MT=1
cd ..
```
Once installed, you can run `make poc` to create the executable file `poc.out`. To use this executable, you can just run `./poc.out` or `./poc.out [nThreads] [compressionLevel]`, where `compressionLevel` goes from 0 (least compressed) to 19 (most compressed).