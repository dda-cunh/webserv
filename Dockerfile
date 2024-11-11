# Use an official GCC image as a parent image
FROM gcc:latest

# Set the working directory in the container
WORKDIR /usr/src/app

# Copy the current directory contents into the container at /usr/src/app
COPY . .

# Install any necessary packages
RUN apt-get update && \
    apt-get install -y locales make && \
    rm -rf /var/lib/apt/lists/*

# Set locale to ensure readable output in logs
RUN locale-gen en_US.UTF-8
ENV LANG=en_US.UTF-8
ENV LANGUAGE=en_US:en
ENV LC_ALL=en_US.UTF-8

# Expose the ports that match docker-compose.yml
EXPOSE 4242 4243

# Run the web server when the container launches
CMD ["sh", "-c", "make re && ./webserv test_files/configs/test.conf"]
