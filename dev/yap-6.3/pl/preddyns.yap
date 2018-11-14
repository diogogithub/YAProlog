                                % The next predicates are applicable only
                                % to dynamic code

/** @file preddyns.yap */

/**
 * @ingroup Database
 * @{

Next follow the main operations on dynamic predicates.

*/

/** @pred  asserta(+ _C_) is iso


Adds clause  _C_ to the beginning of the program. If the predicate is
undefined, it is declared  dynamic (see dynamic/1).

*/
asserta(Clause) :-
    '$assert'(Clause, asserta, _).

/** @pred  assertz(+ _C_) is iso


Adds clause  _C_ to the end of the program. If the predicate is
undefined, it is declared  dynamic (see dynamic/1).

Most Prolog systems only allow asserting clauses for dynamic
predicates. This is also as specified in the ISO standard. YAP also allows
asserting clauses for static predicates, under the restriction that the static predicate may not be live in the stacks.
*/
assertz(Clause) :-
    '$assert'(Clause, assertz, _).

/** @pred  assert(+ _C_)

Same as assertz/1. Adds clause  _C_ to the program. If the predicate is undefined,
declare it as dynamic. New code should use assertz/1 for better portability.

Most Prolog systems only allow asserting clauses for dynamic
predicates. This is also as specified in the ISO standard. YAP allows
asserting clauses for static predicates, as long as the predicate is not
in use and the language flag is <tt>cprolog</tt>. Note that this feature is
deprecated, if you want to assert clauses for static procedures you
should use assert_static/1.

*/
assert(Clause) :-
    '$assert'(Clause, assertz, _).

'$assert'(Clause, Where, R) :-
    '$expand_clause'(Clause,C0,C),
    '$$compile'(C, Where, C0, R).

/** @pred  asserta(+ _C_,- _R_)

The same as `asserta(C)` but unifying  _R_ with
the  database reference that identifies the new clause, in a
one-to-one way. Note that `asserta/2` only works for dynamic
predicates. If the predicate is undefined, it will automatically be
declared dynamic.

*/
asserta(Clause, Ref) :-
    '$assert'(Clause, first, Ref).

/** @pred  assertz(+ _C_,- _R_)

The same as `assertz(C)` but unifying  _R_ with
the  database reference that identifies the new clause, in a
one-to-one way. Note that `asserta/2` only works for dynamic
predicates. If the predicate is undefined, it will automatically be
declared dynamic.


*/
assertz(Clause, Ref) :-
    '$assert'(Clause, last, Ref).

/** @pred  assert(+ _C_,- _R_)

The same as `assert(C)` ( (see Modifying the Database)) but
unifies  _R_ with the  database reference that identifies the new
clause, in a one-to-one way. Note that `asserta/2` only works for dynamic
predicates. If the predicate is undefined, it will automatically be
declared dynamic.


*/
assert(Clause, Ref) :-
    '$assert'(Clause, last, Ref).


'$assertz_dynamic'(X, C, C0, Mod) :-
    (X/\4)=:=0,
    !,
    '$head_and_body'(C,H,B),
    '$assertat_d'(last,H,B,C0,Mod,_).
'$assertz_dynamic'(X,C,C0,Mod) :-
	'$head_and_body'(C,H,B),
	functor(H,N,A),
	('$check_if_reconsulted'(N,A) ->
     true
    ;
     (X/\8)=:=0 ->
     '$inform_as_reconsulted'(N,A),
     '$remove_all_d_clauses'(H,Mod)
    ;
     true
	),
	'$assertat_d'(last,H,B,C0,Mod,_).


'$remove_all_d_clauses'(H,M) :-
	'$is_multifile'(H, M), !,
	functor(H, Na, A),
	'$erase_all_mf_dynamic'(Na,A,M).
'$remove_all_d_clauses'(H,M) :-
	'$recordedp'(M:H,_,R), erase(R), fail.
'$remove_all_d_clauses'(_,_).

'$erase_all_mf_dynamic'(Na,A,M) :-
	source_location( F , _),
	recorded('$multifile_dynamic'(_,_,_), '$mf'(Na,A,M,F,R), R1),
	erase(R1),
	erase(R),
	fail.
'$erase_all_mf_dynamic'(_,_,_).

'$assertat_d'(first,Head,Body,C0,Mod,R) :- !,
	'$compile_dynamic'((Head:-Body), first, C0, Mod, CR),
    ( get_value('$abol',true)
    ->
      '$predicate_flags'(Head,Mod,Fl,Fl),
      ( Fl /\ 0x20000000 =\= 0 -> '$check_multifile_pred'(Head,Mod,Fl) ; true )
    ;
      true
    ),
	'$head_and_body'(C0, H0, B0),
	'$recordap'(Mod:Head,(H0 :- B0),R,CR),
	( '$is_multifile'(Head, Mod) ->
      source_location(F, _),
      functor(H0, Na, Ar),
      recorda('$multifile_dynamic'(_,_,_), '$mf'(Na,Ar,Mod,F,R), _)
	;
      true
	).
'$assertat_d'(last,Head,Body,C0,Mod,R) :-
	'$compile_dynamic'((Head:-Body), last, C0, Mod, CR),
    ( get_value('$abol',true)
    ->
      '$predicate_flags'(Head,Mod,Fl,Fl),
      ( Fl /\ 0x20000000 =\= 0 -> '$check_multifile_pred'(Head,Mod,Fl) ; true )
    ;
      true
    ),
	'$head_and_body'(C0, H0, B0),
	'$recordzp'(Mod:Head,(H0 :- B0),R,CR),
	( '$is_multifile'(H0, Mod) ->
      source_location(F, _),
      functor(H0, Na, Ar),
      recordz('$multifile_dynamic'(_,_,_), '$mf'(Na,Ar,Mod,F,R), _)
	;
      true
	).

/** @pred  retract(+ _C_) is iso


Erases the first clause in the program that matches  _C_. This
predicate may also be used for the static predicates that have been
compiled when the source mode was `on`. For more information on
source/0 ( (see Setting the Compiler)).


*/
retract( C ) :-
    strip_module( C, M, C0),
	'$check_head_and_body'(M:C0,M1,H,B,retract(M:C)),
	'$predicate_flags'(H, M1, F, F),
	'$retract2'(F, H, M1, B,_).

'$retract2'(F, H, M, B, R) :-
	F /\ 0x08000000 =:= 0x08000000, !,
                                %	'$is_log_updatable'(H, M), !,
	'$log_update_clause'(H,M,B,R),
	( F /\ 0x20000000  =:= 0x20000000, recorded('$mf','$mf_clause'(_,_,_,_,R),MR), erase(MR), fail ; true),
	erase(R).
'$retract2'(F, H, M, B, R) :-
                                %	'$is_dynamic'(H,M), !,
	F /\ 0x00002000 =:= 0x00002000, !,
	'$recordedp'(M:H,(H:-B),R),
	( F /\ 0x20000000  =:= 0x20000000, recorded('$mf','$mf_clause'(_,_,_,_,MRef),MR), erase(MR), erase(MRef), fail ; true),
	erase(R).
'$retract2'(_, H,M,_,_) :-
	'$undefined'(H,M), !,
	functor(H,Na,Ar),
	'$dynamic'(Na/Ar,M),
	fail.
'$retract2'(_, H,M,B,_) :-
	functor(H,Na,Ar),
	\+ '$dynamic'(Na/Ar,M),
	'$do_error'(permission_error(modify,static_procedure,Na/Ar),retract(M:(H:-B))).

/** @pred  retract(+ _C_,- _R_)

Erases from the program the clause  _C_ whose
database reference is  _R_. The predicate must be dynamic.




*/
retract(M:C,R) :- !,
    '$yap_strip_module'( C, M, H0),
    '$retract'(H0, M, R).

'$retract'(C, M0, R) :-
	db_reference(R),
    	'$check_head_and_body'(M0:C,M,H,B,retract(C,R)),
	dynamic(H,M),
	!,
	instance(R,(H:-B)),
    erase(R).
'$retract'(C,M0,R) :-
	'$check_head_and_body'(M0:C,M,H,B,retract(C,R)),
	var(R), !,
	'$retract2'(H, M, B, R).
'$retract'(C,M,_) :-
	'$fetch_predicate_indicator_from_clause'(C, M, PI),
    \+ '$dynamic'(PI),
	'$do_error'(permission_error(modify,static_procedure,PI),retract(M:C)).

'$fetch_predicate_indicator_from_clause'((C :- _), M:Na/Ar) :-
!,
    '$yap_strip_module'(C, M, C1),
    functor(C1, Na, Ar).
'$fetch_predicate_indicator_from_clause'(C, M:Na/Ar) :-
    '$yap_strip_module'(C, M, C1),
	functor(C1, Na, Ar).


/** @pred  retractall(+ _G_) is iso


Retract all the clauses whose head matches the goal  _G_. Goal
 _G_ must be a call to a dynamic predicate.

*/
retractall(M:V) :- !,
	'$retractall'(V,M).
retractall(V) :-
	'$current_module'(M),
	'$retractall'(V,M).

'$retractall'(V,M) :- var(V), !,
	'$do_error'(instantiation_error,retract(M:V)).
'$retractall'(M:V,_) :- !,
	'$retractall'(V,M).
'$retractall'(T,M) :-
	(
     '$is_log_updatable'(T, M) ->
	 ( '$is_multifile'(T, M) ->
	   '$retractall_lu_mf'(T,M)
	 ;
	   '$retractall_lu'(T,M)
	 )
	;
     \+ callable(T) ->
     '$do_error'(type_error(callable,T),retractall(T))
	;
     '$undefined'(T,M) ->
     functor(T,Na,Ar),
     '$dynamic'(Na/Ar,M), !
	;
     '$is_dynamic'(T,M) ->
     '$erase_all_clauses_for_dynamic'(T, M)
	;
     functor(T,Na,Ar),
     '$do_error'(permission_error(modify,static_procedure,Na/Ar),retractall(T))
	).

'$retractall_lu'(T,M) :-
	'$free_arguments'(T), !,
	( '$purge_clauses'(T,M), fail ; true ).
'$retractall_lu'(T,M) :-
	'$log_update_clause'(T,M,_,R),
	erase(R),
	fail.
'$retractall_lu'(_,_).

'$retractall_lu_mf'(T,M) :-
	'$log_update_clause'(T,M,_,R),
	( recorded('$mf','$mf_clause'(_,_,_,_,R),MR), erase(MR), fail ; true),
	erase(R),
	fail.
'$retractall_lu_mf'(_,_).

'$erase_all_clauses_for_dynamic'(T, M) :-
	'$recordedp'(M:T,(T :- _),R), erase(R), fail.
'$erase_all_clauses_for_dynamic'(T,M) :-
	'$recordedp'(M:T,_,_), fail.
'$erase_all_clauses_for_dynamic'(_,_).

/* support for abolish/1 */
'$abolishd'(T, M) :-
	'$is_multifile'(T,M),
	functor(T,Name,Arity),
	recorded('$mf','$mf_clause'(_,Name,Arity,M,Ref),R),
	erase(R),
	erase(Ref),
	fail.
'$abolishd'(T, M) :-
	recorded('$import','$import'(_,M,_,T,_,_),R),
	erase(R),
	fail.
'$abolishd'(T, M) :-
	'$purge_clauses'(T,M), fail.
'$abolishd'(T, M) :-
	'$kill_dynamic'(T,M), fail.
'$abolishd'(_, _).


/** @pred  dynamic_predicate(+ _P_,+ _Semantics_)


Declares predicate  _P_ or list of predicates [ _P1_,..., _Pn_]
as a dynamic predicate following either `logical` or
`immediate` semantics.


*/
dynamic_predicate(P,Sem) :-
	'$bad_if_is_semantics'(Sem, dynamic(P,Sem)).
dynamic_predicate(P,Sem) :-
	'$log_upd'(OldSem),
	( Sem = logical -> '$switch_log_upd'(1) ; '$switch_log_upd'(0) ),
	'$current_module'(M),
	'$dynamic'(P, M),
	'$switch_log_upd'(OldSem).

'$bad_if_is_semantics'(Sem, Goal) :-
	var(Sem), !,
	'$do_error'(instantiation_error,Goal).
'$bad_if_is_semantics'(Sem, Goal) :-
	Sem \= immediate, Sem \= logical, !,
	'$do_error'(domain_error(semantics_indicator,Sem),Goal).
