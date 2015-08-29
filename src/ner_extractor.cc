#include <nan.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdlib>
#include <string>

#include <mitie/named_entity_extractor.h>
#include <mitie/conll_tokenizer.h>

#include "ner_extractor.h"

using namespace v8;
using namespace std;
using namespace mitie;

Nan::Persistent<v8::Function> NerExtractor::constructor;

NerExtractor::NerExtractor(string ner_file_path) {
  string classname;
  dlib::deserialize(ner_file_path) >> classname >> ner_;
  tagstr_ = ner_.get_tag_name_strings();
}

NerExtractor::~NerExtractor() {
}

void NerExtractor::Init(v8::Local<v8::Object> exports) {
  Nan::HandleScope scope;

  // Prepare constructor template
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("NerExtractor").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // Prototype
  Nan::SetPrototypeMethod(tpl, "predict", Predict);

  constructor.Reset(tpl->GetFunction());
  exports->Set(Nan::New("NerExtractor").ToLocalChecked(), tpl->GetFunction());
}

void NerExtractor::New(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  if (info.IsConstructCall()) {
    // Invoked as constructor: `new NerExtractor(...)`
    v8::String::Utf8Value arg0(info[0]->ToString());
    string ner_file_path = std::string(*arg0);
    NerExtractor* obj = new NerExtractor(ner_file_path);
    obj->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
  } else {
    // Invoked as plain function `NerExtractor(...)`, turn into construct call.
    const int argc = 1;
    v8::Local<v8::Value> argv[argc] = { info[0] };
    v8::Local<v8::Function> cons = Nan::New<v8::Function>(constructor);
    info.GetReturnValue().Set(cons->NewInstance(argc, argv));
  }
}

void NerExtractor::Predict(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  NerExtractor* obj = ObjectWrap::Unwrap<NerExtractor>(info.Holder());

  v8::String::Utf8Value arg0(info[0]->ToString());
  const std::string data = std::string(*arg0);

  istringstream fin(data);
  conll_tokenizer tok(fin);
  vector<string> tokens;
  string token;
  while(tok(token))
      tokens.push_back(token);

  vector<pair<unsigned long, unsigned long> > chunks;
  vector<unsigned long> chunk_tags;
  vector<double> chunk_scores;

  obj->ner_.predict(tokens, chunks, chunk_tags, chunk_scores);

  v8::Local<v8::Object> resultObj = Nan::New<v8::Object>();
  v8::Local<v8::Array> tags = Nan::New<v8::Array>(chunks.size());

  for (unsigned int i = 0; i < chunks.size(); ++i) {
    v8::Local<v8::Object> tag = Nan::New<v8::Object>();

    v8::Local<v8::Number> tagNumber = Nan::New<v8::Number>(chunk_tags[i]);
    tag->Set(Nan::New("tagNumber").ToLocalChecked(), tagNumber);

    tag->Set(Nan::New("score").ToLocalChecked(), Nan::New<v8::Number>(chunk_scores[i]));

    string chunktag_string(obj->tagstr_[chunk_tags[i]]);

    v8::Local<v8::String> tagType = Nan::New<v8::String>(chunktag_string).ToLocalChecked();
    tag->Set(Nan::New("tagType").ToLocalChecked(), tagType);

    string tokenString("");

    for (unsigned long j = chunks[i].first; j < chunks[i].second; ++j) {
      tokenString += tokens[j];
      if (j < chunks[i].second - 1) {
        tokenString += " ";
      }
    }

    tag->Set(Nan::New("tokens").ToLocalChecked(), Nan::New<v8::String>(tokenString).ToLocalChecked());

    tags->Set(i, tag);
  }

  resultObj->Set(Nan::New("tags").ToLocalChecked(), tags);

  info.GetReturnValue().Set(resultObj);
}
