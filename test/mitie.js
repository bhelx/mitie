var assert = require("assert");
var mitie = require('..');

if (!process.env.NER_MODEL || !process.env.REL_MODEL) {
  console.log("You must provide a NER_MODEL and REL_MODEL env variable");
  process.exit(1);
}

console.log("Using NER model: ", process.env.NER_MODEL);
console.log("Using REL model: ", process.env.REL_MODEL);
console.log();

describe('mitie', function() {
  describe('EntityExtractor', function () {

    it('should contain a EntityExtractor function', function () {
      assert.equal(typeof mitie.EntityExtractor, 'function');
    });

    var extractor = new mitie.EntityExtractor(process.env.NER_MODEL);

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

  describe('RelationExtractor', function () {

    it('should contain a RelationExtractor function', function () {
      assert.equal(typeof mitie.RelationExtractor, 'function');
    });

    var relExtractor = new mitie.RelationExtractor(process.env.NER_MODEL, process.env.REL_MODEL);

    it('should construct the extractor given an entity extractor and model path ', function () {
      assert.equal(typeof relExtractor, 'object');
      assert.equal(typeof relExtractor.predict, 'function');
    });

    var sentence = "Donald Trump says that Barack Obama was born in Kenya, not Hawaii.";
    var results = relExtractor.predict(sentence);

    it('should predict two tags from test sentence "'+sentence+'"' , function () {
      assert(results.tags.length, 2);
    });

    it('should predict tag "Donald Trump" of type "PERSON"', function () {
      var tag = results.tags[0];
      assert(tag.tagType, 'PERSON');
      assert(tag.tokens, 'Donald Trump');
    });

    it('should predict tag "Barack Obama" of type "PERSON"', function () {
      var tag = results.tags[1];
      assert(tag.tagType, 'PERSON');
      assert(tag.tokens, 'Barack Obama');
    });

    it('should predict tag "Kenya" of type "LOCATION"', function () {
      var tag = results.tags[2];
      assert(tag.tagType, 'LOCATION');
      assert(tag.tokens, 'Kenya');
    });

    it('should predict tag "Hawaii" of type "LOCATION"', function () {
      var tag = results.tags[3];
      assert(tag.tagType, 'LOCATION');
      assert(tag.tokens, 'Hawaii');
    });

    it('should predict 1 relation', function () {
      assert(results.relations.length, 1);
    });

    it('should predict relation "Barack Obama <born in> Kenya"', function () {
      var relation = results.relations[0];
      assert(relation.first, 'Barack Obama');
      assert(relation.second, 'Kenya');
    });

  });
});

