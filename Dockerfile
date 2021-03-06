FROM gcr.io/distroless/cc
COPY . /vmd
WORKDIR /vmd/
CMD ["/vmd/vmd"]
