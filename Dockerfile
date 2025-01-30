FROM ubuntu:latest

RUN apt-get update
RUN apt-get install -y g++ wget make libssl-dev unzip git libcogl-pango-dev

WORKDIR /tmp/cmake
RUN wget https://github.com/Kitware/CMake/releases/download/v3.31.4/cmake-3.31.4.tar.gz
RUN tar -xvzf cmake-3.31.4.tar.gz
WORKDIR /tmp/cmake/cmake-3.31.4
RUN ./bootstrap && make && make install

WORKDIR /tmp/opencv
RUN wget -O opencv.zip https://github.com/opencv/opencv/archive/4.x.zip
RUN unzip opencv.zip
WORKDIR /tmp/opencv/build
RUN cmake  ../opencv-4.x
RUN cmake --build .
RUN make install


WORKDIR /tmp
RUN apt-get install -y libtool libleptonica-dev
RUN git clone https://github.com/tesseract-ocr/tesseract.git
WORKDIR /tmp/tesseract
RUN ./autogen.sh
RUN ./configure
WORKDIR /tmp/tesseract/build
RUN cmake ../
RUN cmake --build .
RUN make install

WORKDIR /tmp/leptonica
RUN wget http://www.leptonica.org/source/leptonica-1.85.0.tar.gz
RUN tar -xvzf leptonica-1.85.0.tar.gz
WORKDIR /tmp/leptonica/leptonica-1.85.0/build
RUN cmake ..
RUN cmake --build .
RUN make install
ENV TESSDATA_PREFIX=/usr/local/share/tessdata

#WORKDIR /application

ENTRYPOINT [ "/bin/bash"]