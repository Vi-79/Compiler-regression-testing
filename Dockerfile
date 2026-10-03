FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    gcc-9 gcc-12 \
    clang-11 clang-15 \
    python3 python3-pip \
    make time \
    && rm -rf /var/lib/apt/lists/*

RUN pip3 install matplotlib pandas

WORKDIR /project
COPY . /project

CMD ["python3", "src/runner.py"]