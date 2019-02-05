# Qclassification

A new Python API for Intention classification at Qwant Research.
The API is based on fasttext a c++ toolkit for word embeddings and neural classification.

Contact: c.servan@qwantresearch.com

## Installation

```  git clone https://github.com/QwantResearch/qclassification.git
     git clone https://github.com/QwantResearch/qnlp-toolkit.git
     git clone https://github.com/facebookresearch/fastText.git
  
  sudo -H python3 -m pip --upgrade pytest pybind11 falcon requests json wsgiref falcon falcon_cors 
  
  pushd fastText && mkdir build && cd build && cmake .. && make -j4 && sudo make install && popd
  pushd qnlp-toolkit && mkdir build && cd build && cmake .. && make -j4 && sudo make install && popd
  pushd qclassification/fastText/ && bash compile.bash && popd
  pushd qclassification/libTokenizer/ && bash cbuild.sh && popd
  
``` 

## Launch the API

Set up the models in the file `models_config.txt` and set the IP and desired listened port in the file `qclassif_api.py`.
Then:

```  
  cd qclassif
  python3 qclassif_api.py
``` 

## Query example
```
curl -X POST \
  http://xx.xx.xx.xx:yyyy/classification \
  -H 'Authorization: customized-token' \
  -H 'Cache-Control: no-cache' \
  -H 'Content-Type: application/json' \
  -H 'Postman-Token: 0ba004a2-3b50-5019-d788-32aec5e7cbe3' \
  -d '{"text":"paris reservation chambre","domain":"intention","count":10}'
  ```
which returns:
```
{
     "domain": "intention", 
     "tokenized": "paris reservation chambre", 
     "result": [
          ["hotel", 0.9443409442901611], 
          ["flight", 0.05350146442651749], 
          ["pagesjaunes", 0.001554204965941608], 
          ["others", 0.0005534848314709961], 
          ["maps", 3.014166395587381e-05], 
          ["shopping", 2.9071170501993038e-05], 
          ["music", 2.0554260117933154e-05], 
          ["horaire", 1.9974664610344917e-05], 
          ["fligth", 1.9753684682655148e-05], 
          ["movie", 1.7308637325186282e-05]
     ], 
     "language": "en", 
     "text": "paris reservation chambre", 
     "count": 10
}
```
