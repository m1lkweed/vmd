FROM gcc:latest as builder
COPY . /vmd
WORKDIR /vmd/
RUN make

FROM gcr.io/distroless/cc
COPY --from=builder /vmd/vmd /app/vmd
CMD ["/app/vmd"]