-module(turnos).
-export([server/1]).

% parse divide Msg en tokens, y retorna una tupla con una lista de todos los
% tokens completos, y la primer parte del ultimo mensaje que no esté completo
% Por ej, parse("VO\nNUEVO\nNU") -> {["VO", "NUEVO"], "NU"}
parse(Msg) ->
  Toks = string:split(Msg, "\n", all),
  Len = length(Toks),
  {lists:sublist(Toks, Len - 1), lists:nth(Len, Toks)}.

send_socket(_S, _C, []) -> ""; % Se consumieron todos los mensajes encolados
send_socket(Socket, Counter, [Tok | Toks]) ->
  if
    Tok == "NUEVO" -> N = counters:get(Counter, 1),
                      counters:add(Counter, 1, 1),
                      gen_tcp:send(Socket, integer_to_list(N)), % Se responde al socket
                      send_socket(Socket, Counter, Toks); % Recursivamente respondemos a los otros mensajes
    true -> gen_tcp:send(Socket, "Comando desconocido"),
            io:fwrite("Mensaje desconocido ~p, Socket: ~p~n", [Tok, Socket]), % Un mensaje en el medio no es reconocido
            send_socket(Socket, Counter, Toks)
  end.

server(Active) ->
  {ok, ListenSocket} = gen_tcp:listen(8000, [{reuseaddr, true}, {active, Active}]),
  Counter = counters:new(1, []),
  spawn(fun() -> wait_connect(ListenSocket, Active, Counter) end).

wait_connect(ListenSocket, Active, Counter) ->
  case gen_tcp:accept(ListenSocket) of
    {ok, Socket} ->
                  spawn(fun () -> wait_connect(ListenSocket, Active, Counter) end),
                  case Active of
                    true  -> get_request(Socket, Counter, "");
                    false -> get_request_pasive(Socket, Counter, "")
                  end;
    {error, Reason} -> io:fwrite("ERROR: ~p~n", [Reason])
  end.

get_request(Socket, Counter, MsgBuf) ->
  io:fwrite("Esperando mensajes de ~p~n", [Socket]),
  receive
    {tcp, _, Msg}  -> {Toks, MsgR} = parse(lists:append(MsgBuf, Msg)),
                      send_socket(Socket, Counter, Toks),
                      get_request(Socket, Counter, MsgR);
    {tcp_closed, Port} -> io:fwrite("Cerrando puerto ~p~n", [Port]),
                          gen_tcp:close(Socket)
  end.

% get_request para conexion TCP pasiva
get_request_pasive(Socket, Counter, MsgBuf) ->
  io:fwrite("Esperando mensajes de ~p~n", [Socket]),
  case gen_tcp:recv(Socket, 0) of
    {ok, Msg}  -> {Toks, MsgR} = parse(lists:append(MsgBuf, Msg)),
                  send_socket(Socket, Counter, Toks),
                  get_request_pasive(Socket, Counter, MsgR);
    {error, closed}  -> io:fwrite("Cerrando puerto ~p~n", [Socket]),
                        gen_tcp:close(Socket)
  end.
