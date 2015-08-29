## MITIE

This project is a node binding for the [MIT Information Extraction library](https://github.com/mit-nlp/MITIE). It's written in C++ and js.

I was most interested in using the [Named Entity Recognition](https://en.wikipedia.org/wiki/Named-entity_recognition) extractor so that's all that's been implemented so far.

## Using

The quickest way to get the mitie dependency is with homebrew:

```
brew install mitie
```

You can install with npm on the command line or in your `package.json`:

```
npm install mitie --save
```

The API looks something like this:

```javascript
var mitie = require('mitie');

// Point it at a ner model .dat file
// You can also find where to get these on the main MIT project repo

var nerFile = '/usr/local/Cellar/mitie/0.4/share/MITIE-models/english/ner_model.dat';
var extractor = new mitie.EntityExtractor(nerFile);

// Feed in some data to the predict method

var data = "Barack Obama visited New Orleans today for the anniversary of Hurricane Katrina."
var results = extractor.predict(data);

/**
 * results => {
 *  tags:
 *    [ { tagNumber: 0,
 *        score: 1.0957487452879051,
 *       tagType: 'PERSON',
 *       tokens: 'Barack Obama' },
 *      { tagNumber: 1,
 *        score: 0.8764343236070008,
 *        tagType: 'LOCATION',
 *        tokens: 'New Orleans' } ] }
 */
```

## Testing

Install with development deps:

```
npm install --dev
```

Run mocha tests pointing to a ner model:

```
NER_MODEL=/usr/local/Cellar/mitie/0.4/share/MITIE-models/english/ner_model.dat npm test
```

## TODO

* Would like to implement the rest of the library (implement binary relation detection)
* API could be cleaner for ner extractor, should allow passing a file path for the data
* Support other platforms, will probably not compile on anything other than OS X for now

