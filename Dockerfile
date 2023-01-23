FROM hepstore/rivet-pythia

RUN ( \
    mkdir /code \
    ) 

## Install data science stuff
RUN (\
pip3 install jupyter ipykernel py4j google-common hdfs hdfs3 matplotlib scipy numpy \
     scikit-learn keras tensorflow jupyter metakernel zmq \
     lz4 notebook \
     awkward uproot uproot3-methods uproot3 correctionlib pyarrow fsspec numba tornado \
     coffea pandas neural-structured-learning mplhep packaging cachetools dataclasses hist vector \
)

## Install PythiaGenJets
RUN (\
    cd /code && git clone https://github.com/rappoccio/PythiaGenJets.git \
    && cd PythiaGenJets && source /usr/local/root/bin/thisroot.sh && make pythia2root && mv pythia2root /usr/bin && mv pythia2rootDct_rdict.pcm /usr/bin \
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
