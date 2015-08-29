## MITIE

This project is a node binding for the [MIT Information Extraction library](https://github.com/mit-nlp/MITIE). It's written in C++ and js.

I've implemented both the [Named Entity Recognition](https://en.wikipedia.org/wiki/Named-entity_recognition) extractor and the Binary Relation detector.

## Why?

The purpose of this library is to extract information from free-form text. It allows us to extract two different types of information.

#### Named Entities

Named Entities are typically a (person, location, organization, etc) but you can train your own model to find any type of named entity.

For example, suppose we take the sentence:

> Barack Obama was born in Hawaii and graduated from Harvard Law School in Cambridge Massachusetts.

This library can extract and tag the named entities:

```
person: Barack Obama
location: Hawaii
organization: Harvard Law School
location: Cambridge Massachusetts
```

#### Binary Relations

Binary Relations allow us to predict relations bewteen our named entity.
We must use different models for each relationship (MITIE comes with a number of classifiers but you can also train your own).
As an example, suppose we run our example sentence through the model `binary_relations/rel_classifier_people.person.place_of_birth.svm`.
It would show us that Entity 'Barack Obama' is related to Entity 'Hawaii'.

```
Barack Obama <born in> Hawaii
```

## API

```javascript
var mitie = require('mitie');

// Point it at a ner model .dat file
// You can also find where to get these on the main MIT project repo

var nerFile = '/usr/local/Cellar/mitie/0.4/share/MITIE-models/english/ner_model.dat';
var neExtractor = new mitie.EntityExtractor(nerFile);

// Feed in some data to the predict method

var data = "Barack Obama visited New Orleans today for the anniversary of Hurricane Katrina."
var results = neExtractor.predict(data);

/**
 * results => {
 *  tags:
 *    [ { tagNumber: 0,
 *        score: 1.0957487452879051,
 *        tagType: 'PERSON',
 *        tokens: 'Barack Obama' },
 *      { tagNumber: 1,
 *        score: 0.8764343236070008,
 *        tagType: 'LOCATION',
 *        tokens: 'New Orleans' } ] }
 */

var relFile = '/usr/local/Cellar/mitie/0.4/share/MITIE-models/english/binary_relations/rel_classifier_people.person.place_of_birth.svm';
var relExtractor = new mitie.RelationExtractor(nerFile, relFile);

data = "Donald Trump says that Barack Obama was born in Kenya, not Hawaii.";
results = relExtractor.predict(data);

/**
 * results => {
 *   tags:
 *    [ { tagNumber: 0, tagType: 'PERSON', tokens: 'Donald Trump' },
 *      { tagNumber: 0, tagType: 'PERSON', tokens: 'Barack Obama' },
 *      { tagNumber: 1, tagType: 'LOCATION', tokens: 'Kenya' },
 *      { tagNumber: 1, tagType: 'LOCATION', tokens: 'Hawaii' } ],
 *   relations:
 *    [ { score: 0.5990590608043147,
 *        first: 'Barack Obama',
 *        second: 'Kenya' } ] } 
 */

```

## Installing

The quickest way to get the mitie dependency is with homebrew:

```
brew install mitie
```

You can install with npm on the command line or in your `package.json`:

```
npm install mitie --save
```

## Testing

Install with development deps:

```
npm install --dev
```

Set two env vars however you like (you can also inline):

```bash
export NER_MODEL=/usr/local/Cellar/mitie/0.4/share/MITIE-models/english/ner_model.dat
export REL_MODEL=/usr/local/Cellar/mitie/0.4/share/MITIE-models/english/binary_relations/rel_classifier_people.person.place_of_birth.svm
```

Run mocha tests:

```
npm test
```

## TODO

* No error handling on C++ level
* The the js API is not too good and it's not very efficient
* Consider implementing trainers
