#include <nan.h>
#include "ner_extractor.h"

void InitAll(v8::Local<v8::Object> exports) {
  NerExtractor::Init(exports);
}

NODE_MODULE(mitie, InitAll)
