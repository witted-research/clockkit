FROM arm32v7/ubuntu

COPY qemu-arm-static /usr/bin

RUN apt-get update && apt-get install -y openssh-server sudo
RUN mkdir /var/run/sshd
RUN echo 'root:root' |chpasswd
RUN sed -ri 's/^#?PermitRootLogin\s+.*/PermitRootLogin yes/' /etc/ssh/sshd_config
RUN sed -ri 's/UsePAM yes/#UsePAM yes/g' /etc/ssh/sshd_config
RUN mkdir /root/.ssh

ENV DEBIAN_FRONTEND="noninteractive" TZ="Europe/Rome"
RUN apt -y  update && apt -y install git make g++ libcommoncpp2-dev swig tcl8.6-dev libpython3.8-dev ruby ruby2.7-dev
RUN git clone https://github.com/camilleg/clockkit.git

WORKDIR clockkit/ClockKit
RUN rm SystemClock.cpp
COPY ClockKit/SystemClock.cpp .
RUN make

CMD [ "./ckserver", "4567" ]