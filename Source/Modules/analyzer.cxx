#include "swigmod.h"
#include "cparse.h"
#include <ctype.h>
#include <string>

#define DISABLED 1
#define ENTER 2
#define EXIT  4
#define BEFORE 8
#define AFTER  16

#define ALL (ENTER | EXIT | BEFORE | AFTER)
#define DEFAULT (ENTER | EXIT)

#define NONE 0

#define NFLAGS 20

class ANALYZER : public Language {

public:
    ANALYZER() { _default(); }
    ~ANALYZER() {}

    
    /* Function handlers */

    virtual int functionHandler(Node *n);
    virtual int globalfunctionHandler(Node *n);
    virtual int memberfunctionHandler(Node *n);
    virtual int staticmemberfunctionHandler(Node *n);
    virtual int callbackfunctionHandler(Node *n);

    /* Variable handlers */

    virtual int variableHandler(Node *n);
    virtual int globalvariableHandler(Node *n);
    virtual int membervariableHandler(Node *n);
    virtual int staticmembervariableHandler(Node *n);

    /* C++ handlers */

    virtual int memberconstantHandler(Node *n);
    virtual int constructorHandler(Node *n);
    virtual int copyconstructorHandler(Node *n);
    virtual int destructorHandler(Node *n);
    virtual int classHandler(Node *n);

    /* Miscellaneous */

    //virtual int typedefHandler(Node *n);

    /* Low-level code generation */

    virtual int constantWrapper(Node *n);
    virtual int variableWrapper(Node *n);
    virtual int functionWrapper(Node *n);
    virtual int nativeWrapper(Node *n);

    /* main and top */
    virtual void main(int argc, char *argv[]);
    virtual int top(Node *n);

    static enum Flags {
        _constantWrapper = 0,
        _variableWrapper,
        _nativeWrapper,
        _functionWrapper,
        _functionHandler,
        _globalfunctionHandler,
        _memberfunctionHandler,
        _staticmemberfunctionHandler,
        _destructorHandler,
        _copyconstructorHandler,
        _classHandler,
        _variableHandler,
        _globalvariableHandler,
        _membervariableHandler,
        _staticmembervariableHandler,
        _memberconstantHandler,
        _constructorHandler,
        _callbackfunctionHandler,
        _top        
    } Flags;

private:
    
    unsigned int flags[NFLAGS];
    
    void _all() {
        for(unsigned int i=0; i<NFLAGS; ++i) {
            flags[i] = ALL;
        }
    }

    void _none() {
        for(unsigned int i=0; i<NFLAGS; ++i) {
            flags[i] = NONE;
        }
    }

    void _default() {
        for(unsigned int i=0; i<NFLAGS; ++i) {
            flags[i] = DEFAULT;
        }
    }

};

#define QSTR(s) STR(s)
#define STR(s) #s

#define ENTER_MSG "enter %s() of %s\n"
#define EXIT_MSG "exit %s() of %s\n"

#define PRINT_ENTER_MSG(f)     if(flags[_##f]&ENTER) { \
        Printf(stdout, ENTER_MSG, STR(f), Getattr(n, "name"));\
}

#define PRINT_NODE_BEFORE(f) if(flags[_##f]&BEFORE) { \
        Swig_print_node(n);\
}

#define PRINT_NODE_AFTER(f) if(flags[_##f]&AFTER) { \
        Swig_print_node(n);\
}

#define PRINT_EXIT_MSG(f) if(flags[_##f]&EXIT) { \
        Printf(stdout, EXIT_MSG, STR(f), Getattr(n, "name"));\
}

#define PRINT_MESSAGES(f, call) { \
    PRINT_ENTER_MSG(f);\
    PRINT_NODE_BEFORE(f);\
    call;\
    PRINT_NODE_AFTER(f);\
    PRINT_EXIT_MSG(f);\
}

/* ---------------------------------------------------------------------
 * constantWrapper()
 *
 * Low level code generator for constants 
 * --------------------------------------------------------------------- */
int ANALYZER::constantWrapper(Node *n) {
    PRINT_MESSAGES(constantWrapper, 
    Language::constantWrapper(n)
    );
    return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * variableWrapper()
 *
 * Low level code generator for variables  
 * --------------------------------------------------------------------- */

int ANALYZER::variableWrapper(Node *n) {
    PRINT_MESSAGES(variableWrapper, 
    Language::variableWrapper(n)
    );
    return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * nativeWrapper()
 * --------------------------------------------------------------------- */

int ANALYZER::nativeWrapper(Node *n) {
    PRINT_MESSAGES(nativeWrapper, 
    Language::nativeWrapper(n)
    );
    return SWIG_OK;
}


/* ---------------------------------------------------------------------
 * functionWrapper()
 *
 * Low level code generator for functions 
 * --------------------------------------------------------------------- */

int ANALYZER::functionWrapper(Node *n) {
    PRINT_MESSAGES(functionWrapper, 
    Language::functionWrapper(n)
    );
    return SWIG_OK;
}


/* ---------------------------------------------------------------------
 * functionHandler()
 *
 * Function handler for generating wrappers for functions 
 * --------------------------------------------------------------------- */

int ANALYZER::functionHandler(Node *n) {
    PRINT_MESSAGES(functionHandler, 
    Language::functionHandler(n)
    );
    return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * globalfunctionHandler()
 *
 * Function handler for generating wrappers for functions 
 * --------------------------------------------------------------------- */

int ANALYZER::globalfunctionHandler(Node *n) {
    PRINT_MESSAGES(globalfunctionHandler, 
    Language::globalfunctionHandler(n)
    );
    return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * memberfunctionHandler()
 *
 * Function handler for generating wrappers for functions 
 * --------------------------------------------------------------------- */

int ANALYZER::memberfunctionHandler(Node *n) {
    PRINT_MESSAGES(memberfunctionHandler, 
    Language::memberfunctionHandler(n)
    );
    return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * staticmemberfunctionHandler()
 *
 * Function handler for generating wrappers for static member functions 
 * --------------------------------------------------------------------- */

int ANALYZER::staticmemberfunctionHandler(Node *n) {
    PRINT_MESSAGES(staticmemberfunctionHandler, 
    Language::staticmemberfunctionHandler(n)
    );
    return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * callbackfunctionHandler()
 *
 * Function handler for generating wrappers for callbacks 
 * --------------------------------------------------------------------- */

int ANALYZER::callbackfunctionHandler(Node *n) {
    PRINT_MESSAGES(callbackfunctionHandler, 
    Language::callbackfunctionHandler(n)
    );
    return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * variableHandler()
 *
 * Function handler for generating wrappers for variables 
 * --------------------------------------------------------------------- */

int ANALYZER::variableHandler(Node *n) {
    PRINT_MESSAGES(variableHandler, 
    Language::variableHandler(n)
    );
    return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * globalvariableHandler()
 *
 * Function handler for generating wrappers for global variables 
 * --------------------------------------------------------------------- */

int ANALYZER::globalvariableHandler(Node *n) {
    PRINT_MESSAGES(globalvariableHandler, 
    Language::globalvariableHandler(n)
    );
    return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * membervariableHandler()
 *
 * Function handler for generating wrappers for member variables 
 * --------------------------------------------------------------------- */

int ANALYZER::membervariableHandler(Node *n) {
    PRINT_MESSAGES(membervariableHandler, 
    Language::membervariableHandler(n)
    );
    return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * staticmembervariableHandler()
 *
 * Function handler for generating wrappers for static member variables 
 * --------------------------------------------------------------------- */

int ANALYZER::staticmembervariableHandler(Node *n) {
    PRINT_MESSAGES(staticmembervariableHandler, 
    Language::staticmembervariableHandler(n)
    );
    return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * memberconstantHandler()
 *
 * Function handler for generating wrappers for member constant 
 * --------------------------------------------------------------------- */

int ANALYZER::memberconstantHandler(Node *n) {
    PRINT_MESSAGES(memberconstantHandler, 
    Language::memberconstantHandler(n)
    );
    return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * constructorHandler()
 *
 * Function handler for generating wrappers for constructor 
 * --------------------------------------------------------------------- */

int ANALYZER::constructorHandler(Node *n) {    
    PRINT_MESSAGES(constructorHandler, 
    Language::constructorHandler(n)
    );
    return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * copyconstructorHandler()
 *
 * Function handler for generating wrappers for copy constructor 
 * --------------------------------------------------------------------- */

int ANALYZER::copyconstructorHandler(Node *n) {
    PRINT_MESSAGES(copyconstructorHandler, 
    Language::copyconstructorHandler(n)
    );
    return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * destructorHandler()
 *
 * Function handler for generating wrappers for destructor
 * --------------------------------------------------------------------- */

int ANALYZER::destructorHandler(Node *n) {
    PRINT_MESSAGES(destructorHandler, 
    Language::destructorHandler(n)
    );
    return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * classHandler()
 *
 * Function handler for generating wrappers for class 
 * --------------------------------------------------------------------- */

int ANALYZER::classHandler(Node *n) {
    PRINT_MESSAGES(classHandler, 
    Language::classHandler(n)
    );
    return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * top()
 *
 * Function handler for processing top node of the parse tree 
 * Wrapper code generation essentially starts from here 
 * --------------------------------------------------------------------- */

int ANALYZER::top(Node *n) {
    PRINT_MESSAGES(top, 
    Language::top(n)
    );
    return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * main()
 *
 * Entry point for the ANALYZER module
 * --------------------------------------------------------------------- */

void ANALYZER::main(int argc, char *argv[]) {

    unsigned int current_flag = _top;
    
    for (int i = 1; i < argc; i++) {
        if (argv[i]) {
            if (strcmp(argv[i], "all") == 0) {
                Swig_mark_arg(i);
                _all();
            } else if (strcmp(argv[i], "none") == 0) {
                Swig_mark_arg(i);
                _none();
            } else if (strcmp(argv[i], "constantWrapper") == 0) {
                Swig_mark_arg(i);
                current_flag = _constantWrapper;
            } else if (strcmp(argv[i], "variableWrapper") == 0) {
                Swig_mark_arg(i);
                current_flag = _variableWrapper;
            } else if (strcmp(argv[i], "nativeWrapper") == 0) {
                Swig_mark_arg(i);
                current_flag = _nativeWrapper;
            } else if (strcmp(argv[i], "functionWrapper") == 0) {
                Swig_mark_arg(i);
                current_flag = _functionWrapper;
            } else if (strcmp(argv[i], "functionHandler") == 0) {
                Swig_mark_arg(i);
                current_flag = _functionHandler;
            } else if (strcmp(argv[i], "globalfunctionHandler") == 0) {
                Swig_mark_arg(i);
                current_flag = _globalfunctionHandler;
            } else if (strcmp(argv[i], "memberfunctionHandler") == 0) {
                Swig_mark_arg(i);
                current_flag = _memberfunctionHandler;
            } else if (strcmp(argv[i], "staticmemberfunctionHandler") == 0) {
                Swig_mark_arg(i);
                current_flag = _staticmemberfunctionHandler;
            } else if (strcmp(argv[i], "destructorHandler") == 0) {
                Swig_mark_arg(i);
                current_flag = _destructorHandler;
            } else if (strcmp(argv[i], "copyconstructorHandler") == 0) {
                Swig_mark_arg(i);
                current_flag = _copyconstructorHandler;
            } else if (strcmp(argv[i], "classHandler") == 0) {
                Swig_mark_arg(i);
                current_flag = _classHandler;
            } else if (strcmp(argv[i], "variableHandler") == 0) {
                Swig_mark_arg(i);
                current_flag = _variableHandler;
            } else if (strcmp(argv[i], "globalvariableHandler") == 0) {
                Swig_mark_arg(i);
                current_flag = _globalvariableHandler;
            } else if (strcmp(argv[i], "membervariableHandler") == 0) {
                Swig_mark_arg(i);
                current_flag = _membervariableHandler;
            } else if (strcmp(argv[i], "staticmembervariableHandler") == 0) {
                Swig_mark_arg(i);
                current_flag = _staticmembervariableHandler;
            } else if (strcmp(argv[i], "memberconstantHandler") == 0) {
                Swig_mark_arg(i);
                current_flag = _memberconstantHandler;
            } else if (strcmp(argv[i], "constructorHandler") == 0) {
                Swig_mark_arg(i);
                current_flag = _constructorHandler;
            } else if (strcmp(argv[i], "callbackfunctionHandler") == 0) {
                Swig_mark_arg(i);
                current_flag = _callbackfunctionHandler;
            } else if (strcmp(argv[i], "top") == 0) {
                Swig_mark_arg(i);
                current_flag = _top;
            } else if (strcmp(argv[i], "+before") == 0) {
                Swig_mark_arg(i);
                flags[current_flag]|=BEFORE;
            } else if (strcmp(argv[i], "-before") == 0) {
                Swig_mark_arg(i);
                flags[current_flag]= (flags[current_flag]^BEFORE)&flags[current_flag];
            } else if (strcmp(argv[i], "+after") == 0) {
                Swig_mark_arg(i);
                flags[current_flag]|=AFTER;
            } else if (strcmp(argv[i], "-after") == 0) {
                Swig_mark_arg(i);
                flags[current_flag]= (flags[current_flag]^AFTER)&flags[current_flag];
            }
        }
    }

    allow_overloading();
}

/* -----------------------------------------------------------------------------
 * swig_analyzer()    - Instantiate module
 * ----------------------------------------------------------------------------- */

static Language *new_swig_analyzer() {
  return new ANALYZER();
}

extern "C" Language *swig_analyzer(void) {
  return new_swig_analyzer();
}
