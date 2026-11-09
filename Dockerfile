FROM alpine
WORKDIR /lab
COPY /build/main /lab/main
RUN chmod +x /lab/main
ENTRYPOINT ["/lab/main"]