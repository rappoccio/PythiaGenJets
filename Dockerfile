FROM hepstore/rivet-pythia

## Get ROOT
RUN ( \
    cd /code && wget https://root.cern/download/root_v6.22.00.Linux-ubuntu20-x86_64-gcc9.3.tar.gz \
    && tar -zxvf root_v6.22.00.Linux-ubuntu20-x86_64-gcc9.3.tar.gz \
    && source /code/root/bin/thisroot.sh \
    )

## Install data science stuff
RUN (\
pip3 install jupyter ipykernel py4j google-common hdfs hdfs3 matplotlib scipy numpy \
     scikit-learn keras tensorflow jupyter metakernel zmq \
     lz4 notebook uproot tornado coffea pandas neural-structured-learning \
)

## Install PythiaGenJets
RUN (\
    cd /code && git clone https://github.com/rappoccio/PythiaGenJets.git \
    && cd PythiaGenJets && source /code/root/bin/thisroot.sh && make pythia2root && mv pythia2root /usr/bin && mv pythia2rootDct_rdict.pcm /usr/bin \
)

# Create a user that does not have root privileges 
ARG username=physicist
ENV MY_UID 1000
RUN useradd --create-home --home-dir /home/${username} --uid ${MY_UID} ${username}
ENV HOME /home/${username}

# Set the cwd to /home/{username}
WORKDIR /home/${username}

# Switch to our newly created user
USER ${username}


COPY . ${HOME}
USER root
RUN chown -R ${MY_UID} ${HOME}
USER ${username}

# Allow incoming connections on port 8888
EXPOSE 8888

# Run notebook when the container launches
CMD ["/bin/bash"]
