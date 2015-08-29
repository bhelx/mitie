#include <nan.h>
#include "entity_extractor.h"

void InitAll(v8::Local<v8::Object> exports) {
  EntityExtractor::Init(exports);
}

NODE_MODULE(mitie, InitAll)
