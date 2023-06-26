-module(sync).
-export([createLock/0, lock/1, unlock/1, destroyLock/1]).
-export([createSem/1, semP/1, semV/1, destroySem/1]).
-export([testLock/0, testSem/0]).

createLock () -> spawn(fun() -> lockLoop() end).

lockLoop() ->
  receive
    {lockSig, Pid} -> Pid ! pass, % Se da permiso a algun proceso con intenciones de tomar el lock
                      receive     % que puede seguir (este proceso es quien "tiene" el lock)
                        unlockSig -> lockLoop() % Se espera al aviso de que el lock se ha soltado
                      end;
    endLock -> ok % No se usa mas el lock, el proceso puede terminar
  end.

lock (Lock) ->
  Lock ! {lockSig, self()}, % Se avisa al proceso actuando de lock la intención de tomar el lock
  receive
    pass -> ok % Se espera a la señal para pasar
  end.

unlock (Lock) -> Lock ! unlockSig.

destroyLock (Lock) -> Lock ! endLock.

createSem (N) -> spawn(fun() -> semLoop(N) end).

semLoop (0) -> receive post -> semLoop(1) end; % Solo se recibe post si el contador es 0
semLoop (N) when N > 0 ->
  receive
    post -> semLoop(N + 1);
    {wait, Pid} -> Pid ! pass, % Respondemos al proceso para que pueda seguir
                   semLoop(N - 1)
  end.

destroySem (S) -> S ! endSem.

semP (S) ->
  S ! {wait, self()}, % Avisamos al semaforo la intencion de disminuir el valor
  receive
    pass -> ok % Nos responde con la señal para poder seguir
  end.

semV (S) -> S ! post.

f (L, W) ->
  lock(L),
  % regioncritica(),
  io:format("uno ~p~n", [self()]),
  io:format("dos ~p~n", [self()]),
  io:format("tre ~p~n", [self()]),
  io:format("cua ~p~n", [self()]),
  unlock(L),
  W ! finished.

waiter (L, 0) -> destroyLock(L);
waiter (L, N) -> receive finished -> waiter(L, N-1) end.

waiter_sem (S, 0) -> destroySem(S);
waiter_sem (S, N) -> receive finished -> waiter_sem(S, N-1) end.

testLock () ->
  L = createLock(),
  W = spawn(fun () -> waiter(L, 3) end),
  spawn (fun () -> f(L, W) end),
  spawn (fun () -> f(L, W) end),
  spawn (fun () -> f(L, W) end),
  ok.

sem (S, W) ->
  semP(S),
  %regioncritica(), bueno, casi....
  io:format("uno ~p~n", [self()]),
  io:format("dos ~p~n", [self()]),
  io:format("tre ~p~n", [self()]),
  io:format("cua ~p~n", [self()]),
  io:format("cin ~p~n", [self()]),
  io:format("sei ~p~n", [self()]),
  semV(S),
  W ! finished.

testSem () ->
  S = createSem(2), % a lo sumo dos usando io al mismo tiempo
  W = spawn (fun () -> waiter_sem (S, 5) end),
  spawn (fun () -> sem (S, W) end),
  spawn (fun () -> sem (S, W) end),
  spawn (fun () -> sem (S, W) end),
  spawn (fun () -> sem (S, W) end),
  spawn (fun () -> sem (S, W) end),
  ok.
