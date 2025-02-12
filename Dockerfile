FROM debian:latest

# Install Valgrind
RUN apt update && apt install -y valgrind gcc gdb make

# Set the working directory
WORKDIR /workspace

# Keep the container running
CMD ["/bin/bash"]
