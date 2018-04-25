import json
import logging
import sys
import uuid
from wsgiref import simple_server
import FastText as ft

import falcon
import requests
# import apply_bpe
import codecs
import qtokenizer
from falcon_cors import CORS

globalcors=CORS(allow_all_origins=True,allow_all_headers=True,allow_all_methods=True)


inet='localhost'
iport=8888


class CLASSIFEngine(object):
    
    cors = globalcors
    def __init__(self):
        self._classifmodel={}
        self._tokenizer={}
        # sys.stderr.write("SET MODEL: "+str(language)+"\n Model: "+model_filename+"\n")

        
    def classification_function(self, text,language):
        sys.stderr.write("ASK CLASSIFICATION: "+str(language)+" ||| "+text+"\n")
        l_classification_results=[]
        if language in self._classifmodel:
            l_labels,l_probs = self._classifmodel[str(language)].predict([text.strip()])
            for i in range(len(l_labels)):
                l_classification_results.append([l_labels[i][0],l_probs[i][0]])
            print (l_labels)
            print (l_classification_results)
            #sys.stderr.write("CLASSIFICATION: "+str(language)+" ||| "+text+" ||| "+str(l_classification_results)+"\n")
            return l_classification_results
        else:
            sys.stderr.write("CLASSIFICATION ERROR: "+str(language)+" ||| "+text+"\n")
            return []

    def set_classification_model(self,model_filename,language):
        sys.stderr.write("SET MODEL: "+str(language)+"\n Model: "+model_filename+"\n")
        self._classifmodel[str(language)] = ft.load_model(model_filename)
        self._tokenizer[str(language)] = qtokenizer.qtokenizer(language)


    def tokenize(self, text,language):
        sys.stderr.write("TOKENIZE: "+str(language)+" ||| "+text+"\n")
        if language in self._tokenizer:
            l_tokenized=self._tokenizer[language].tokenize_str(text.strip())
            sys.stderr.write(l_tokenized+"\n")
            return l_tokenized
        else:
            sys.stderr.write("TOKENIZE ERROR: "+str(language)+" ||| "+text+"\n")
            return "None"

    def get_languages(self):
        l_lp=[]
        for l_keys in self._classifmodel.keys():
            l_lp.append(l_keys)
        return l_lp

class AuthMiddleware(object):
    cors = globalcors
    def process_request(self, req, resp):
        token = req.get_header('Authorization')
        account_id = req.get_header('Account-ID')

        challenges = ['Token type="Fernet"']

        if token is None:
            description = ('Please provide an auth token '
                           'as part of the request.')
            print (description)
            raise falcon.HTTPUnauthorized('Auth token required',
                                          description,
                                          challenges,
                                          href='http://docs.example.com/auth')

        if not self._token_is_valid(token, account_id):
            description = ('The provided auth token is not valid. '
                           'Please request a new token and try again.')

            raise falcon.HTTPUnauthorized('Authentication required',
                                          description,
                                          challenges,
                                          href='http://docs.example.com/auth')

    def _token_is_valid(self, token, account_id):
        return True  # Suuuuuure it's valid...


class RequireJSON(object):

    cors = globalcors
    def process_request(self, req, resp):
        if not req.client_accepts_json:
            raise falcon.HTTPNotAcceptable(
                'This API only supports responses encoded as JSON.',
                href='http://docs.examples.com/api/json')

        if req.method in ('POST', 'PUT'):
            if 'application/json' not in req.content_type:
                raise falcon.HTTPUnsupportedMediaType(
                    'This API only supports requests encoded as JSON.',
                    href='http://docs.examples.com/api/json')


class JSONTranslator(object):
    # NOTE: Starting with Falcon 1.3, you can simply
    # use req.media and resp.media for this instead.

    cors = globalcors
    def process_request(self, req, resp):
        # req.stream corresponds to the WSGI wsgi.input environ variable,
        # and allows you to read bytes from the request body.
        #
        # See also: PEP 3333
        if req.content_length in (None, 0):
            # Nothing to do
            return

        body = req.stream.read()
        if not body:
            raise falcon.HTTPBadRequest('Empty request body',
                                        'A valid JSON document is required.')

        try:
            req.context['doc'] = json.loads(body.decode('utf-8'))

        except (ValueError, UnicodeDecodeError):
            raise falcon.HTTPError(falcon.HTTP_753,
                                   'Malformed JSON',
                                   'Could not decode the request body. The '
                                   'JSON was incorrect or not encoded as '
                                   'UTF-8.')

    def process_response(self, req, resp, resource):
        if 'result' not in resp.context:
            return

        resp.body = json.dumps(resp.context['result'])


def max_body(limit):

    def hook(req, resp, resource, params):
        length = req.content_length
        if length is not None and length > limit:
            msg = ('The size of the request is too large. The body must not '
                   'exceed ' + str(limit) + ' bytes in length.')

            raise falcon.HTTPRequestEntityTooLarge(
                'Request body is too large', msg)

    return hook


class ClassificationResource(object):
    cors = globalcors
    def __init__(self, classifmodel):
        self.logger = logging.getLogger('qclassif_api.' + __name__)
        self.classifmodel = classifmodel 


    @falcon.before(max_body(64 * 1024))
    def on_post(self, req, resp):
        try:
            doc = req.context['doc']
        except KeyError:
            raise falcon.HTTPBadRequest(
                'Missing thing',
                'A thing must be submitted in the request body.')
        if len(doc["text"]) > 0:
            doc["tokenized"]=self.classifmodel.tokenize(doc["text"],doc["language"])
            doc["result"]=[]
            doc["result"].append({"generic_model_"+doc["language"]:[self.classifmodel.classification_function(doc["tokenized"],doc["language"])]})
        else:
            doc["tokenized"]=""
            doc["result"]=[]
        resp.context['result'] = doc
        resp.set_header('Powered-By', 'Qwant Research')
        resp.status = falcon.HTTP_202


class LanguageResource(object):
    cors = globalcors   
    def __init__(self, classifmodel):
        self.logger = logging.getLogger('qclassif_api.' + __name__)
        self.classifmodel = classifmodel 
    
    def get_language_data(self):
        return {
        "de": ['Allemand', 'de'],
        "en": ['Anglais', 'gb'],
        "es": ['Espagnol', 'es'],
        "fr": ['Français', 'fr'],
        "nl": ['Néerlandais', 'nl'],
        "pt": ['Portugais', 'pt'],
        "cs": ['Tchèque', 'cz'],
        "it": ['Italien', 'it']
        }


    @falcon.before(max_body(64 * 1024))
    def on_get(self, req, resp):
        resp.set_header('Powered-By', 'Qwant Research')
        resp.status = falcon.HTTP_200
        doc={}
        doc["language_available"]=[]
        doc["language_available"]=self.classifmodel.get_languages()
        doc["languages"]={}
        doc["languages"]=self.get_language_data()
        resp.context['result'] = doc
        resp.set_header('Powered-By', 'Qwant Research')
        # resp.set_header('Access-Control-Allow-Methods', 'GET')
        # resp.set_header('Access-Control-Allow-Headers','Content-Type, Access-Control-Allow-Headers, Authorization, X-Requested-With')

    


app = falcon.API(middleware=[globalcors.middleware,RequireJSON(),JSONTranslator()])

fconfig = open("models_config.txt","r")

classificationEngine = CLASSIFEngine()
for lines in fconfig:
    l_data=lines.split()
    classificationEngine.set_classification_model(l_data[1],l_data[0])

    
classification = ClassificationResource(classificationEngine)
languages= LanguageResource(classificationEngine)

app.add_route('/classification',classification)
app.add_route('/languages',languages)

if __name__ == '__main__':
    httpd = simple_server.make_server(inet, iport, app)
    sys.stderr.write("Classification server online on "+str(inet)+":"+str(iport)+"\n")
    httpd.serve_forever()

