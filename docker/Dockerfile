FROM gcc:latest
RUN useradd -m myuser
WORKDIR /usr/src/app
COPY ../ ./
RUN apt-get update && \
    apt-get install -y locales make && \
    rm -rf /var/lib/apt/lists/*
RUN mkdir -p /usr/src/app/temp && \
    chown -R myuser:myuser /usr/src/app/temp && \
    chmod -R 777 /usr/src/app/temp
RUN ls -lR /usr/src/app/temp
USER myuser
CMD ["sh", "-c", "make re && ./webserv configs/test.conf"]
