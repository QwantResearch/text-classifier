# Qclassification

A new CPP API for Intention classification at Qwant Research.
The API is based on fasttext.

Contact: christophe[dot]servan[at]qwantresearch[dot]com

## Installation

### Pre-requieres
* install qnlp-toolkit (https://github.com/QwantResearch/qnlp-toolkit.git)
* install fasttext (https://github.com/facebookresearch/fastText.git)
* install forked version of pistache (https://github.com/QwantResearch/pistache.git)
* install nlohmann json (https://github.com/nlohmann/json.git)


### Installation


```
  git clone https://github.com/QwantResearch/qclassification.git 
  git checkout cpp
  mkdir -pv build && cd build && cmake .. && make 
``` 


## Launch the API

Launch the API:
```
  ./api_classification [#port] [#threads] [config filename]
``` 

