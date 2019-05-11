# Prom

A simple collection of functions for working with Javascript promises (`Js.Promise`) in ReasonML.

# API

## Prom.make

Use this function to turn a callback into a promise. We can call it to get back
a tuple whose first value is a promise that can be passed around, and second
value is a function you can call to resolve that promise.

```reasonml
open Prom;

let thingThatReturnsAPromise = () => {
  let (p, resolve) = make();
  thingThatTakesACallback(thingWeWaitedFor => {
    resolve(thingWeWaitedFor)
  });
  p;
}
```

## Prom.map

"map" is another term for Javascript's `.then` function on a promise. It
changes the value inside of a promise into a different value.

```reasonml
open Prom;

// We're talking to a pretend HTTP service that gives us information
// about our fishes.
getNumberOfFishes()
->map(fishesCount => fishesCount * 5);
```

## Prom.flatMap or Prom.andThen

"flatMap" (aka "andThen") is another term for Javascript's `.then` function on a promise. While
"map" changes a value inside a promise, "flatMap" lets you chain promises together,
ultimately ending up with just one promise, and potentially changing the value
along the way.

```reasonml
open Prom;

// It seems the cost of feeding fish doesn't scale linearly, so we have to
// first get the number of fishes we have, and then ask the fishes service
// how much it'll cost to feed them.
getNumberOfFishes()
->flatMap(fishesCount => getCostOfFoodForFishCount(fishesCount));
```

## Prom.catch

"catch" Just gives us a chance to recover from a rejected promise.

```reasonml
open Prom;

getNumberOfFishes()
->(catch(err => {
  Js.log2("Failed to get fish count from primary service, trying backup", err)
  getNumberOfFishesFromBackupService()
}));
```

## Prom.wrap

"wrap" Takes a value and converts it into a promise of a value, that's all.

```reasonml
open Prom;

let fishCount = 16

thingThatExpectsAPromiseOfFishCount(wrap(fishCount));
```

## Prom.wait

"wait" Is there for when we want to just do something with the result of a value,
like printing it to the console, rather than transforming it, or chaining another
promise off of it.

```reasonml
open Prom;

getNumberOfFishes()
->wait(Js.log);
```

## Prom.all

"wait" Is there for when we want to just do something with the result of a value,
like printing it to the console, rather than transforming it, or chaining another
promise off of it.

```reasonml
open Prom;

// Calling this returns a promise of an array of fish details.
let getFishDetails = (fishIds: array(string)) => {
  all(fishIds->Belt.Array.map(getFishDetailsForId))
}
```
