#include <nan.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdlib>
#include <string>

#include <mitie/binary_relation_detector.h>
#include <mitie/named_entity_extractor.h>
#include <mitie/conll_tokenizer.h>

#include "relation_extractor.h"
#include "entity_extractor.h"

using namespace v8;
using namespace std;
using namespace mitie;

string cat_tokens (
    const std::vector<std::string>& tokens,
    const std::pair<unsigned long, unsigned long>& range
)
{
    // Note that the entities are identified using half open ranges.  So we loop over all
    // the elements of the supplied half open range and concatenate them.
    string temp;
    for (unsigned long i = range.first; i < range.second; ++i)
    {
        temp += tokens[i];
        if (i+1 < range.second)
            temp += " ";
    }
    return temp;
}

Nan::Persistent<v8::Function> RelationExtractor::constructor;

RelationExtractor::RelationExtractor(string ner_file, string relation_file) {
  ner_file_ = ner_file;
  relation_file_ = relation_file;
}

RelationExtractor::~RelationExtractor() {
}

void RelationExtractor::Init(v8::Local<v8::Object> exports) {
  Nan::HandleScope scope;

  // Prepare constructor template
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("RelationExtractor").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // Prototype
  Nan::SetPrototypeMethod(tpl, "predict", Predict);

  constructor.Reset(tpl->GetFunction());
  exports->Set(Nan::New("RelationExtractor").ToLocalChecked(), tpl->GetFunction());
}

void RelationExtractor::New(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  if (info.IsConstructCall()) {
    // Invoked as constructor: `new RelationExtractor(...)`

    v8::String::Utf8Value arg0(info[0]->ToString());
    string ner_file = std::string(*arg0);

    v8::String::Utf8Value arg1(info[1]->ToString());
    string relation_file = std::string(*arg1);

    RelationExtractor* obj = new RelationExtractor(ner_file, relation_file);

    obj->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
  } else {
    // Invoked as plain function `RelationExtractor(...)`, turn into construct call.
    const int argc = 1;
    v8::Local<v8::Value> argv[argc] = { info[0] };
    v8::Local<v8::Function> cons = Nan::New<v8::Function>(constructor);
    info.GetReturnValue().Set(cons->NewInstance(argc, argv));
  }
}

void RelationExtractor::Predict(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  RelationExtractor* obj = ObjectWrap::Unwrap<RelationExtractor>(info.Holder());

  string classname;
  named_entity_extractor ner;
  dlib::deserialize(obj->ner_file_) >> classname >> ner;

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

  ner(tokens, chunks, chunk_tags);

  vector<string> tagstr = ner.get_tag_name_strings();

  // create tags first
  v8::Local<v8::Array> tags = Nan::New<v8::Array>(chunks.size());

  for (unsigned int i = 0; i < chunks.size(); ++i) {
    v8::Local<v8::Object> tag = Nan::New<v8::Object>();

    v8::Local<v8::Number> tagNumber = Nan::New<v8::Number>(chunk_tags[i]);
    tag->Set(Nan::New("tagNumber").ToLocalChecked(), tagNumber);

    string chunktag_string(tagstr[chunk_tags[i]]);

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

  v8::Local<v8::Object> resultObj = Nan::New<v8::Object>();

  resultObj->Set(Nan::New("tags").ToLocalChecked(), tags);

  // detect relations

  v8::Local<v8::Array> relations = Nan::New<v8::Array>();

  binary_relation_detector bd;
  dlib::deserialize(obj->relation_file_) >> classname >> bd;

  unsigned int count = 0;
  for (unsigned long i = 0; i+1 < chunks.size(); ++i)
  {
    // Calling this function runs the relation detector and returns a score value.
    // If the score is > 0 then the detector is indicating that this relation
    // mention is an example of the type of relation this detector is looking for.
    // Moreover, the larger the score the more confident the detector is that this
    // is a correct relation detection.  Here we print anything with a score > 0.
    double score = bd(extract_binary_relation(tokens, chunks[i], chunks[i+1], ner.get_total_word_feature_extractor()));
    if (score > 0) {
      v8::Local<v8::Object> relation = Nan::New<v8::Object>();

      relation->Set(Nan::New("score").ToLocalChecked(), Nan::New<v8::Number>(score));

      v8::Local<v8::String> first = Nan::New<v8::String>(cat_tokens(tokens, chunks[i])).ToLocalChecked();
      relation->Set(Nan::New("first").ToLocalChecked(), first);

      v8::Local<v8::String> second = Nan::New<v8::String>(cat_tokens(tokens, chunks[i+1])).ToLocalChecked();
      relation->Set(Nan::New("second").ToLocalChecked(), second);

      relations->Set(count++, relation);
    }

    // Relations have an ordering to their arguments.  So even if the above
    // relation check failed we still might have a valid relation if we try
    // swapping the two arguments.  So that's what we do here.
    score = bd(extract_binary_relation(tokens, chunks[i+1], chunks[i], ner.get_total_word_feature_extractor()));
    if (score > 0) {
      v8::Local<v8::Object> relation = Nan::New<v8::Object>();

      relation->Set(Nan::New("score").ToLocalChecked(), Nan::New<v8::Number>(score));

      v8::Local<v8::String> first = Nan::New<v8::String>(cat_tokens(tokens, chunks[i+1])).ToLocalChecked();
      relation->Set(Nan::New("first").ToLocalChecked(), first);

      v8::Local<v8::String> second = Nan::New<v8::String>(cat_tokens(tokens, chunks[i])).ToLocalChecked();
      relation->Set(Nan::New("second").ToLocalChecked(), second);

      relations->Set(count++, relation);
    }
  }

  resultObj->Set(Nan::New("relations").ToLocalChecked(), relations);

  info.GetReturnValue().Set(resultObj);
}
