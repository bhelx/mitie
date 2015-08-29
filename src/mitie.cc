#include <nan.h>
#include "entity_extractor.h"
#include "relation_extractor.h"

void InitAll(v8::Local<v8::Object> exports) {
  EntityExtractor::Init(exports);
  RelationExtractor::Init(exports);
}

NODE_MODULE(mitie, InitAll)
