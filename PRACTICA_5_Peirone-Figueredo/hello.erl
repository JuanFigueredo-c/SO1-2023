-module(hello).
-export([init/0, hello/0]).

hello() ->
  receive after 1000 -> ok end,
  io:fwrite("Hello ~p~n", [case rand:uniform(10) of 10 -> 1/uno; _ -> self() end]),
  ?MODULE:hello().

rescue() ->
  % Se crea un subproceso que ejecutara la funcion hello del modulo hello
  % Este subproceso estara enlazado al proceso que ejecuta rescue
  spawn_link(fun() -> ?MODULE:hello() end),
  receive
    {'EXIT', _, _} -> rescue()
  end.

init() ->
  % El proceso será avisado cuando algun proceso enlazado a él muera
  spawn(fun() -> process_flag(trap_exit, true), rescue() end).
