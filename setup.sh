sudo apt-get update

sudo apt-get install -y \
     build-essential \
     libgl1-mesa-dev \
     qt6-base-dev \
     sqlite3 \
     git \
     cmake

cd ..

git clone https://github.com/google/googletest.git -b v1.15.2 ;\
    cd googletest ;\
    mkdir build ;\
    cd build ;\
    cmake .. ;\
    make ;\
    sudo make install
