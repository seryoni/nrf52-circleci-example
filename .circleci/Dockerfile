FROM circleci/python:2-stretch
RUN cd ~ && \
    wegt https://developer.arm.com/-/media/Files/downloads/gnu-rm/7-2018q2/gcc-arm-none-eabi-7-2018-q2-update-linux.tar.bz && \
    tar xvf gcc-arm-none-eabi-7-2018-q2-update-linux.tar.bz && \
    rm -rf gcc-arm-none-eabi-7-2018-q2-update/arm-none-eabi gcc-arm-none-eabi-7-2018-q2-update/lib gcc-arm-none-eabi-7-2018-q2-update/share gcc-arm-none-eabi-7-2018-q2-update-linux.tar.bz
