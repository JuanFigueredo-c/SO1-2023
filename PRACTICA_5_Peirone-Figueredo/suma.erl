-module(suma).
-export([sumaAnillo/1, suma/4, start/1]).

sumaAnillo(N) ->
  Val = 1, % Valor inicial de cada nodo del anillo
  receive
    NextPid -> suma(N - 1, 0, Val, NextPid)
  end.

suma(0, Buf, Val, _N) -> io:fwrite("Pid: ~p, Valor: ~p~n", [self(), Buf + Val]);
suma(N, Buf, Val, Next) ->
  Next ! Buf + Val,
  receive
    M -> suma(N - 1, M, Val, Next)
  end.

start(N) ->
  Pids = [spawn(suma, sumaAnillo, [N]) || _ <- lists:seq(1, N)],
  NextPids = lists:append(lists:nthtail(1, Pids), [lists:nth(1, Pids)]),
  lists:foreach(fun({Pid, Next}) -> Pid ! Next end, lists:zip(Pids, NextPids)).
