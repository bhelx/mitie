var assert = require("assert");
var mitie = require('..');

if (!process.env.NER_MODEL) {
  console.log("You must provide a NER_MODEL env variable potining to the path of the NER model data");
  process.exit(1);
}

describe('mitie', function() {
  describe('NerExtractor', function () {

    it('should contain a NerExtractor class', function () {
      assert.equal(typeof mitie.NerExtractor, 'function');
    });

    var extractor = new mitie.NerExtractor(process.env.NER_MODEL);

    it('should construct the extractor given a model file path', function () {
      assert.equal(typeof extractor, 'object');
      assert.equal(typeof extractor.predict, 'function');
    });

    var sentence = "Barack Obama visited New Orleans today for the anniversary of Hurricane Katrina.";
    var results = extractor.predict(sentence);

    it('should predict two tags from test sentence "'+sentence+'"' , function () {
      assert(results.tags.length, 2);
    });

    it('should predict tag "Barack Obama" of type "PERSON"', function () {
      var tag = results.tags[0];
      assert(tag.tagType, 'PERSON');
      assert(tag.tokens, 'Barack Obama');
    });

    it('should predict tag "New Orleans" of type "LOCATION"', function () {
      var tag = results.tags[1];
      assert(tag.tagType, 'LOCATION');
      assert(tag.tokens, 'New Orleans');
    });
  });
});

