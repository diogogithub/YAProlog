% This example is adapted from http://eclipse.crosscoreop.com/examples/puzzle1.pl.txt

:- use_module(library('clpfd')).

solve(Board) :-
	Board = [NW,N,NE,W,E,SW,S,SE],
	maplist(in_board(0..12), Board),
	sum(Board, #=, 12),
	NW + N + NE #= 5,
	NE + E + SE #= 5,
	NW + W + SW #= 5,
	SW + S + SE #= 5,
	label(Board).

in_board(D, V) :- V in D.
