type prom('a) = Js.Promise.t('a);
type promise('a) = prom('a);

// Use this function to turn a callback into a promise.
// Call it to get back a tuple of (p, re).
// You can use the "p", which is a promise, as the return value of your function,
// and use the "re" function to resolve the function with some value inside of
// your callback.
let make = () => {
  let resolver = ref(ignore);
  let p =
    Js.Promise.make((~resolve, ~reject as _) =>
      resolver := (a => resolve(. a))
    );

  (p, resolver^);
};

let map = (p: Js.Promise.t('a), mapper: 'a => 'b): Js.Promise.t('b) =>
  p |> Js.Promise.then_(v => mapper(v)->Js.Promise.resolve);

let flatMap =
    (p: Js.Promise.t('a), mapper: 'a => Js.Promise.t('b)): Js.Promise.t('b) =>
  p |> Js.Promise.then_(v => mapper(v));

let andThen = flatMap;

let catch =
    (p: Js.Promise.t('a), mapper: Js.Promise.error => Js.Promise.t('a))
    : Js.Promise.t('a) =>
  p |> Js.Promise.catch(mapper);

let wrap: 'a => Js.Promise.t('a) = Js.Promise.resolve;

// For using when you want to perform some effect on the result of a function, instead of
// just transforming the value. (For example, printing the result to the console).
let wait = (p: Js.Promise.t('a), waiter: 'a => unit): Js.Promise.t(unit) =>
  p->map(v => waiter(v));

let all: array(Js.Promise.t('a)) => Js.Promise.t(array('a)) = Js.Promise.all;

module Result = {
  type promWithError('data, 'err) = prom(Belt.Result.t('data, 'err));

  let wrapOk = (a: 'a): promWithError('a, 'err) => wrap(Belt.Result.Ok(a));
  let wrapError = (err: 'err): promWithError('a, 'err) =>
    wrap(Belt.Result.Error(err));

  let mapOk =
      (p: promWithError('a, 'err), mapper: 'a => 'b)
      : promWithError('b, 'err) => {
    p->map(
      fun
      | Belt.Result.Ok(a) => Belt.Result.Ok(mapper(a))
      | Belt.Result.Error(_) as r => r,
    );
  };

  let flatMapOk =
      (p: promWithError('a, 'err), mapper: 'a => promWithError('b, 'err))
      : promWithError('b, 'err) => {
    p->flatMap(
      fun
      | Belt.Result.Ok(a) => mapper(a)
      | Belt.Result.Error(_) as r => r->wrap,
    );
  };

  let mapError =
      (p: promWithError('a, 'err), mapper: 'err => 'err2)
      : promWithError('a, 'err2) => {
    p->map(
      fun
      | Belt.Result.Ok(_) as r => r
      | Belt.Result.Error(_) as r => Belt.Result.map(r, mapper),
    );
  };

  let flatMapError =
      (p: promWithError('a, 'err), mapper: 'err => promWithError('a, 'err2))
      : promWithError('a, 'err2) => {
    p->flatMap(
      fun
      | Belt.Result.Ok(_) as r => r->wrap
      | Belt.Result.Error(e) => mapper(e),
    );
  };

  let recover =
      (
        p: promWithError('a, 'err),
        recoverer: 'err => promWithError('a, 'err),
      )
      : promWithError('b, 'err2) => {
    p->flatMap(
      fun
      | Belt.Result.Ok(_) as r => r->wrap
      | Belt.Result.Error(e) => recoverer(e),
    );
  };
};
include Result;

module Infix = {
  let (<$>) = map;
  let (>>=) = flatMap;
  let (<$$>) = mapOk;
  let (<!!>) = mapError;
  let ($$>=) = flatMapOk;
  let (!!>=) = flatMapError;
  let (-!) = catch;
  let (-|) = wait;
};

let let_ = flatMap;