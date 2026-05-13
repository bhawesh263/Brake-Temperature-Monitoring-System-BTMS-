# Multi-stage build for a slim Go binary
FROM golang:1.26-alpine AS builder

WORKDIR /app

# Download dependencies
COPY go.mod go.sum ./
RUN go mod download

# Copy source and build
COPY . .
RUN CGO_ENABLED=0 GOOS=linux go build -o main .

# Final stage
FROM alpine:latest
WORKDIR /root/
COPY --from=builder /app/main .

EXPOSE 8080

# Run the binary
CMD ["./main"]
