
#define YAP_CPP_DB_INTERFACE 1

//! @{

/**
 *
 *   @defgroup yap-cplus-db-interface Data-Base Component of YAP interface.
 *
 *   @ingroup yap-cplus-interface
 *    @tableofcontents
 *
 *
 * Data-base component of C++ interface to YAP. It manipulates sets of
 * atoms, each one containing a number of props.
 */

class YAPTerm;

class YAPError;

class YAPModule;

/**
 * @brief YAPModule
 * A YAPModule describes a bare module, which in YAP is just a name.
 *
 * Info about the module is in YAPModuleProp
 *
 */
class YAPModule : protected YAPAtomTerm {
  friend class YAPPredicate;
  YAPModule( Term t ): YAPAtomTerm( t ) {};
  Term t() { return gt(); }
  Term curModule() {CACHE_REGS return Yap_CurrentModule(); }
public:
  ~YAPModule( ) {};
  YAPModule( ): YAPAtomTerm( curModule() ) {};
  YAPModule( YAPAtom t ): YAPAtomTerm( t ) {};
};

/**
 * @brief YAPModuleProp
 * A YAPModuleProp controls access to a module property.
 *
 */
class YAPModuleProp: public YAPProp {
  friend class YAPPredicate;
  ModEntry *m;

  YAPModuleProp(ModEntry *mod)   {m = mod;};
  YAPModuleProp(Term tmod)  { m = Yap_GetModuleEntry(tmod); };
public:
  YAPModuleProp()  { CACHE_REGS m = Yap_GetModuleEntry(Yap_CurrentModule()); };
  YAPModuleProp(YAPModule tmod) ;
  virtual YAPModule module() { return YAPModule(m->AtomOfME); };
};

/**
 * @brief YAPFunctor represents Prolog functors Name/Arity
 */
    class YAPFunctor: public YAPProp {
    friend class YAPApplTerm;
    friend class YAPPredicate;
    Functor f;
    /// Constructor: receives Prolog functor and casts it to YAPFunctor
    ///
    /// Notice that this is designed for internal use only.
      YAPFunctor( Functor ff)  { f = ff; }
    public:
      ~YAPFunctor( )  {  };
    /// Constructor: receives name as a string plus arity
    ///
    /// Notice that this is designed for ISO-LATIN-1 right now
      YAPFunctor( char * s, arity_t arity)  { f = Yap_MkFunctor( Yap_LookupAtom( s ), arity ); }
    /// Constructor: receives name as a  wide string plus arity
    ///
    /// Notice that this is designed for UNICODE right now
      YAPFunctor( wchar_t * s, arity_t arity) { f = Yap_MkFunctor( Yap_LookupWideAtom( s ), arity ) ; }
    /// Constructor: receives name as an atom, plus arity
    ///
    /// This is the default method, and the most popi;at
      YAPFunctor( YAPAtom at, arity_t arity)  { f = Yap_MkFunctor( at.a, arity ); }

    /// Getter: extract name of functor as an atom
    ///
    /// this is for external usage.
    YAPAtom name(void) {
      return YAPAtom( NameOfFunctor( f ) );
    }

    /// Getter: extract arity of functor as an unsigned integer
    ///
    /// this is for external usage.
      arity_t arity(void) {
	return ArityOfFunctor( f );
      }

  };

/**
 * @brief Predicates
 *
 * This class interfaces with PredEntry in Yatom.
 */
class YAPPredicate: public YAPModuleProp {
  friend class YAPQuery;

protected:
  PredEntry *ap;

  /// auxiliary routine to find a predicate in the current module.
  PredEntry  *getPred( Term &t, Term* &outp ) ;

  /// String constructor for predicates
  ///
  /// It also communicates the array of arguments t[]
  /// and the array of variables
  /// back to yapquery
  YAPPredicate(const char *s, Term &out, Term &names ) {
    CACHE_REGS
    BACKUP_MACHINE_REGS();
    Term *outp;
    
  out = Yap_StringToTerm(s, strlen(s)+1, &LOCAL_encoding, 1200, &names ) ;
  //extern char *s0;
    //fprintf(stderr,"ap=%p arity=%d text=%s", ap, ap->ArityOfPE, s);
  //  Yap_DebugPlWrite(out);
      //  delete [] ns;
if (out == 0L)
      throw YAPError(SYNTAX_ERROR);
    ap = getPred( out, outp);
    RECOVER_MACHINE_REGS();
  }


  /// Term constructor for predicates
  ///
  /// It is just a call to getPred
  inline YAPPredicate(Term t) {
    CELL *  v = NULL;
    ap = getPred( t , v );
  }

  /// Cast constructor for predicates,
  /// if we have the implementation data.
  ///
  inline YAPPredicate(PredEntry *pe) {
    ap = pe;
  }

public:
  ~YAPPredicate() {};

  /// Functor constructor for predicates
  ///
  /// Asssumes that we use the current module.
  YAPPredicate(YAPFunctor f)  {
    CACHE_REGS
    ap = RepPredProp(PredPropByFunc(f.f,Yap_CurrentModule()));
  };

  /// Functor constructor for predicates, is given a specific module.
  ///
  inline YAPPredicate(YAPFunctor f, YAPTerm mod) {
    ap = RepPredProp(PredPropByFunc(f.f,mod.t));
  }

  /// Name/arity constructor for predicates.
  ///
  inline YAPPredicate(YAPAtom at, YAPTerm mod) {
    ap = RepPredProp(PredPropByAtom(at.a,mod.t));
  }


  /// Name/0 constructor for predicates.
  ///
  YAPPredicate(YAPAtom at);

  /// Mod:Name/Arity constructor for predicates.
  ///
  inline YAPPredicate(YAPAtom at, arity_t arity, YAPModule mod) {
    if (arity) {
      Functor f = Yap_MkFunctor(at.a, arity);
      ap = RepPredProp(PredPropByFunc(f,mod.t()));
    } else {
      ap = RepPredProp(PredPropByAtom(at.a,mod.t()));
    }
  }

  /// Atom/Arity constructor for predicates.
  ///
  YAPPredicate(YAPAtom at, arity_t arity);


  /// module of a predicate
  ///
  /// notice that modules are currently treated as atoms, this should change.
  YAPModule module() {
    if (ap->ModuleOfPred == PROLOG_MODULE)
      return  YAPModule(AtomProlog);
    else
      return  YAPModule(AtomOfTerm(ap->ModuleOfPred));
  }

  /// name of predicate
  ///
  /// notice that we return the atom, not a string.
  YAPAtom name() { if (ap->ArityOfPE)
      return  YAPAtom((Atom)ap->FunctorOfPred);
    else
      return  YAPAtom(NameOfFunctor(ap->FunctorOfPred));
  }

  /// arity of predicate
  ///
  /// we return a positive number.
  arity_t getArity() { return ap->ArityOfPE; }
};

/**
 * @brief PrologPredicate
 *
 * This class interfaces with Predicates Implemented in Prolog.
 */
class YAPPrologPredicate: public YAPPredicate {
public:
  YAPPrologPredicate(YAPAtom name,
		     arity_t arity,
		     YAPModule module = YAPModule(),
		     bool tabled = false,
		     bool logical_updates = false,
                     bool local = false,
		     bool sourced = true,
		     bool discontiguous = false,
                     bool multiFile = false,
                     bool hidden = false,
		     bool untraceable = false,
		     bool unspyable = false,
                     bool meta = false,
                     bool sync = false,
		     bool quasi_quotable = false,
		     size_t mega_clause = 0
		     );
  void *assertClause( YAPTerm clause, bool last=true, YAPTerm source= YAPTerm(TermNil));
  void *retractClause( YAPTerm skeleton, bool all=false);
  void *clause( YAPTerm skeleton, YAPTerm &body );
};

/**
 * @brief PrologPredicate
 *
 * This class interfaces with Predicates Implemented in Prolog.
 */
class YAPFLIP: public YAPPredicate {
public:
  YAPFLIP(CPredicate call,
	  YAPAtom name,
	  arity_t arity,
	  YAPModule module = YAPModule(),
	  CPredicate retry = 0,
	  CPredicate cut = 0,
	  size_t extra = 0,
	  bool test = false
	  ) : YAPPredicate( name, arity, module) {
    if (retry) {
      Yap_InitCPredBackCut(name.getName(), arity, extra, call, retry, cut, UserCPredFlag);
    } else {
      if (test) {
        YAP_UserCPredicate (name.getName(),
                            call, arity);
      } else {
        YAP_UserCPredicate (name.getName(),
                            call, arity);
      }
    }

  };
  YAPFLIP(const char *name,
	  arity_t arity,
          YAPModule module = YAPModule(),
	  bool backtrackable = false
	  ) : YAPPredicate( YAPAtom(name), arity, module) {
    if (backtrackable) {
      Yap_InitCPredBackCut(name, arity, 0, 0, 0, 0, UserCPredFlag);
    } else {
      YAP_UserCPredicate (name,
                            0, arity);
     }
  };
  bool addCall(CPredicate call) {
    return Yap_AddCallToFli( ap, call );
  }
  bool addRetry(CPredicate call) {
    return Yap_AddRetryToFli( ap, call );
  }
  bool addCut(CPredicate call) {
    return Yap_AddCutToFli( ap, call );
  }

};
