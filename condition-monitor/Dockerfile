# Build stage
FROM alpine:latest AS builder

# Install build dependencies
RUN apk add --no-cache \
    build-base \
    cmake \
    git \
    curl-dev

WORKDIR /app

# Copy source code
COPY . .

# Build the project
RUN mkdir build && cd build && \
    cmake .. && \
    make

# Runtime stage
FROM alpine:latest
RUN apk add --no-cache libstdc++ curl

WORKDIR /root/
COPY --from=builder /app/build/monitor_sim .
# We might need headers if any dynamically loaded, but here it's a static build mostly
# However, we need curl for the 'system("curl ...")' call in main.cpp
RUN apk add --no-cache curl

CMD ["./monitor_sim"]
