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