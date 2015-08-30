#ifndef RELATION_EXTRACTOR_H
#define RELATION_EXTRACTOR_H

#include <nan.h>
#include <mitie/named_entity_extractor.h>
#include <mitie/binary_relation_detector.h>

#include "entity_extractor.h"

class RelationExtractor : public Nan::ObjectWrap {
  public:
    static void Init(v8::Local<v8::Object> exports);

  private:
    explicit RelationExtractor(string ner_file, string relation_file);
    ~RelationExtractor();

    static void New(const Nan::FunctionCallbackInfo<v8::Value>& info);
    static void Predict(const Nan::FunctionCallbackInfo<v8::Value>& info);
    static Nan::Persistent<v8::Function> constructor;
    string ner_file_;
    string relation_file_;
};

#endif
