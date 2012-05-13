#include "swigmod.h"
#include "cparse.h"
#include <ctype.h>
#include <string>

class ANALYZER : public Language {

public:
    ANALYZER() { _none(); }
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

private:

    void _all() {
        _enter=1;
        _exit=1;
        _node=1;
        _before=1;
        _after=1;

        _functionHandler=1;
        _globalfunctionHandler=1;
        _memberfunctionHandler=1;
        _staticmemberfunctionHandler=1;
        _destructorHandler=1;
        _copyconstructorHandler=1;
        _classHandler=1;
    }

    void _none() {
        _enter=0;
        _exit=0;
        _node=0;
        _before=0;
        _after=0;

        _functionHandler=0;
        _globalfunctionHandler=0;
        _memberfunctionHandler=0;
        _staticmemberfunctionHandler=0;
        _destructorHandler=0;
        _copyconstructorHandler=0;
        _classHandler=0;
    }

private:
    bool _enter;
    bool _exit;
    bool _node;

    bool _before;
    bool _after;

    bool _functionHandler;
    bool _globalfunctionHandler;
    bool _memberfunctionHandler;
    bool _staticmemberfunctionHandler;
    bool _destructorHandler;
    bool _copyconstructorHandler;
    bool _classHandler;

};

#define QSTR(s) STR(s)
#define STR(s) #s

#define ENTER_MSG "enter %s() of %s.\n"
#define EXIT_MSG "exit %s().of %s\n"

#define PRINT_ENTER_MSG(f)     if(_enter) { \
        Printf(stdout, ENTER_MSG, STR(f), Getattr(n, "name"));\
}

#define PRINT_NODE_BEFORE() if(_before && _node) { \
        Swig_print_node(n);\
}

#define PRINT_NODE_AFTER() if(_after && _node) { \
        Swig_print_node(n);\
}

#define PRINT_EXIT_MSG(f) if(_exit) { \
        Printf(stdout, EXIT_MSG, STR(f), Getattr(n, "name"));\
}


/* ---------------------------------------------------------------------
 * constantWrapper()
 *
 * Low level code generator for constants 
 * --------------------------------------------------------------------- */
int ANALYZER::constantWrapper(Node *n) {
    Language::constantWrapper(n);
    return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * variableWrapper()
 *
 * Low level code generator for variables  
 * --------------------------------------------------------------------- */

int ANALYZER::variableWrapper(Node *n) {
    Language::variableWrapper(n);
    return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * nativeWrapper()
 * --------------------------------------------------------------------- */

int ANALYZER::nativeWrapper(Node *n) {
    Language::nativeWrapper(n);
    return SWIG_OK;
}


/* ---------------------------------------------------------------------
 * functionWrapper()
 *
 * Low level code generator for functions 
 * --------------------------------------------------------------------- */

int ANALYZER::functionWrapper(Node *n) {
    Language::functionWrapper(n);
    return SWIG_OK;
}


/* ---------------------------------------------------------------------
 * functionHandler()
 *
 * Function handler for generating wrappers for functions 
 * --------------------------------------------------------------------- */

int ANALYZER::functionHandler(Node *n) {
    PRINT_ENTER_MSG("functionHandler");
    PRINT_NODE_BEFORE();
    Language::functionHandler(n);
    PRINT_NODE_AFTER();
    PRINT_EXIT_MSG("functionHandler");
    return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * globalfunctionHandler()
 *
 * Function handler for generating wrappers for functions 
 * --------------------------------------------------------------------- */

int ANALYZER::globalfunctionHandler(Node *n) {
    PRINT_ENTER_MSG("globalfunctionHandler");
    PRINT_NODE_BEFORE();
    Language::globalfunctionHandler(n);
    PRINT_NODE_AFTER();
    PRINT_EXIT_MSG("globalfunctionHandler");
    return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * memberfunctionHandler()
 *
 * Function handler for generating wrappers for functions 
 * --------------------------------------------------------------------- */

int ANALYZER::memberfunctionHandler(Node *n) {
    PRINT_ENTER_MSG("memberfunctionHandler");
    PRINT_NODE_BEFORE();
    Language::memberfunctionHandler(n);
    PRINT_NODE_AFTER();
    PRINT_EXIT_MSG("memberfunctionHandler");
    return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * staticmemberfunctionHandler()
 *
 * Function handler for generating wrappers for static member functions 
 * --------------------------------------------------------------------- */

int ANALYZER::staticmemberfunctionHandler(Node *n) {
    PRINT_ENTER_MSG("staticmemberfunctionHandler");
    PRINT_NODE_BEFORE();
    Language::staticmemberfunctionHandler(n);
    PRINT_NODE_AFTER();
    PRINT_EXIT_MSG("staticmemberfunctionHandler");

    return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * callbackfunctionHandler()
 *
 * Function handler for generating wrappers for callbacks 
 * --------------------------------------------------------------------- */

int ANALYZER::callbackfunctionHandler(Node *n) {
    PRINT_ENTER_MSG("callbackfunctionHandler");
    PRINT_NODE_BEFORE();
    Language::callbackfunctionHandler(n);
    PRINT_NODE_AFTER();
    PRINT_EXIT_MSG("callbackfunctionHandler");

    return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * variableHandler()
 *
 * Function handler for generating wrappers for variables 
 * --------------------------------------------------------------------- */

int ANALYZER::variableHandler(Node *n) {
    PRINT_ENTER_MSG("variableHandler");
    PRINT_NODE_BEFORE();
    Language::variableHandler(n);
    PRINT_NODE_AFTER();
    PRINT_EXIT_MSG("variableHandler");

    return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * globalvariableHandler()
 *
 * Function handler for generating wrappers for global variables 
 * --------------------------------------------------------------------- */

int ANALYZER::globalvariableHandler(Node *n) {
    PRINT_ENTER_MSG("globalvariableHandler");
    PRINT_NODE_BEFORE();
    Language::globalvariableHandler(n);
    PRINT_NODE_AFTER();
    PRINT_EXIT_MSG("globalvariableHandler");
    return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * membervariableHandler()
 *
 * Function handler for generating wrappers for member variables 
 * --------------------------------------------------------------------- */

int ANALYZER::membervariableHandler(Node *n) {
    PRINT_ENTER_MSG("membervariableHandler");
    PRINT_NODE_BEFORE();
    Language::membervariableHandler(n);
    PRINT_NODE_AFTER();
    PRINT_EXIT_MSG("membervariableHandler");

    return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * staticmembervariableHandler()
 *
 * Function handler for generating wrappers for static member variables 
 * --------------------------------------------------------------------- */

int ANALYZER::staticmembervariableHandler(Node *n) {
    PRINT_ENTER_MSG("staticmembervariableHandler");
    PRINT_NODE_BEFORE();
    Language::staticmembervariableHandler(n);
    PRINT_NODE_AFTER();
    PRINT_EXIT_MSG("staticmembervariableHandler");
    return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * memberconstantHandler()
 *
 * Function handler for generating wrappers for member constant 
 * --------------------------------------------------------------------- */

int ANALYZER::memberconstantHandler(Node *n) {
    PRINT_ENTER_MSG("memberconstantHandler");
    PRINT_NODE_BEFORE();
    Language::memberconstantHandler(n);
    PRINT_NODE_AFTER();
    PRINT_EXIT_MSG("memberconstantHandler");
    return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * constructorHandler()
 *
 * Function handler for generating wrappers for constructor 
 * --------------------------------------------------------------------- */

int ANALYZER::constructorHandler(Node *n) {    
    PRINT_ENTER_MSG("constructorHandler");
    PRINT_NODE_BEFORE();
    Language::constructorHandler(n);
    PRINT_NODE_AFTER();
    PRINT_EXIT_MSG("constructorHandler");
    return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * copyconstructorHandler()
 *
 * Function handler for generating wrappers for copy constructor 
 * --------------------------------------------------------------------- */

int ANALYZER::copyconstructorHandler(Node *n) {
    PRINT_ENTER_MSG("copyconstructorHandler");
    PRINT_NODE_BEFORE();
    Language::copyconstructorHandler(n);
    PRINT_NODE_AFTER();
    PRINT_EXIT_MSG("copyconstructorHandler");

    return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * destructorHandler()
 *
 * Function handler for generating wrappers for destructor
 * --------------------------------------------------------------------- */

int ANALYZER::destructorHandler(Node *n) {
    PRINT_ENTER_MSG("destructorHandler");
    PRINT_NODE_BEFORE();
    Language::destructorHandler(n);
    PRINT_NODE_AFTER();
    PRINT_EXIT_MSG("destructorHandler");
    return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * classHandler()
 *
 * Function handler for generating wrappers for class 
 * --------------------------------------------------------------------- */

int ANALYZER::classHandler(Node *n) {
    PRINT_ENTER_MSG("classHandler");
    PRINT_NODE_BEFORE();
    Language::classHandler(n);
    PRINT_NODE_AFTER();
    PRINT_EXIT_MSG("classHandler");
    return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * top()
 *
 * Function handler for processing top node of the parse tree 
 * Wrapper code generation essentially starts from here 
 * --------------------------------------------------------------------- */

int ANALYZER::top(Node *n) {
    Language::top(n);
    return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * main()
 *
 * Entry point for the ANALYZER module
 * --------------------------------------------------------------------- */

void ANALYZER::main(int argc, char *argv[]) {

    for (int i = 1; i < argc; i++) {
        if (argv[i]) {
            if (strcmp(argv[i], "-all") == 0) {
                Swig_mark_arg(i);
                _all();
            } else if (strcmp(argv[i], "-enter") == 0) {
                Swig_mark_arg(i);
                _enter=1;
            } else if (strcmp(argv[i], "-exit") == 0) {
                Swig_mark_arg(i);
                _exit=1;
            } else if (strcmp(argv[i], "-before") == 0) {
                Swig_mark_arg(i);
                _before=1;
            } else if (strcmp(argv[i], "-nobefore") == 0) {
                Swig_mark_arg(i);
                _before=0;
            } else if (strcmp(argv[i], "-after") == 0) {
                Swig_mark_arg(i);
                _after=1;
            } else if (strcmp(argv[i], "-noafter") == 0) {
                Swig_mark_arg(i);
                _after=0;
            } else if (strcmp(argv[i], "-node") == 0) {
                Swig_mark_arg(i);
                _node=1;
            } else if (strcmp(argv[i], "-nonode") == 0) {
                Swig_mark_arg(i);
                _node=0;
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
