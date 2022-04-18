% Zad�n� �. 31:
% Napi�te program �e��c� �kol dan� predik�tem u31(LIN,VOUT), kde LIN je vstupn�
% ��seln� seznam s nejm�n� jedn�m prvkem a VOUT je prom�nn�, ve kter� se vrac�
% index prvn�ho v�skytu maxim�ln�ho ��sla v seznamu LIN (indexov�n� za��n�
% jedni�kou).

% RESENI
first(X, [X|_], 1).
first(X, [_|T], I) :- first(X, T, I1), I is I1 + 1.

max([M], M).
max([H|T], M) :- max(T, TM), H>TM, M is H.
max([H|T], M) :- max(T, TM), H=<TM, M is TM.

firstmax(LIN, VOUT) :-
    max(LIN, M),
    first(M, LIN, VOUT).


% Testovac� predik�ty:                                  	% LOUT
u31_1:- u31([5,3,-18,2,-9,-13,17,4],VOUT),write(VOUT).		% 7
u31_2:- u31([5,3.1,17,2,-9.4,-13,17,4], VOUT),write(VOUT).	% 3
u31_3:- u31([5,3.3],VOUT),write(VOUT).				% 1
u31_r:- write('Zadej LIN: '),read(LIN),
	u31(LIN,VOUT),write(VOUT).

u31(LIN,VOUT):-
    firstmax(LIN, VOUT).

