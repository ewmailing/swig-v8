#include "swigmod.h"
#include "cparse.h"
#include <ctype.h>
#include <string>

class JAVASCRIPT : public Language {
private:
    File *f_wrap_h;
    File *f_wrap_cpp;               /* General DOH objects used for holding the strings */
    File *f_runtime;
    File *f_header;
    File *f_wrappers;
    File *f_init;
 	
	String *wrap_h_code;			// code to be written in wrap_h
	String *js_static_values_code;	// code for initializing the staticvalues field of JSClassDefinition
	String *js_static_funcs_code;	// code for initializing the staticfunctions field of JSClassDefinition
	String *js_static_consts_code;	// code for initializing the staticvalues field of the cconst object (for constants)
	String *js_static_cvar_code;	// code for initializing the staticvalues field of the cvar object (for C global variables)
	String *js_create_cpp_object_code; 	// code for initializing the body of the createcppObject function
	String *js_consts_decl_code;	// code for declaration of the 'C' functions.

	String *current_class_name;		// string representing name of the current class.
	String *current_class_type;		// current class when used as a type. This represents the complete name of the class including the scope prefix
	String *variable_name;			// string representing the current variable name.

	bool member_variable_flag;		// flag used when wrapping member variables
	bool static_member_variable_flag;  // flag used when wrapping static member variables
	bool global_variable_flag;		// flag used when wrapping global variables	
	
	bool static_member_func_flag;	// flag used when wrapping static member functions
	bool member_func_flag;			// flag used when wrapping member functions
	bool member_constant_flag;		// flag used when wrapping member constants
	
	bool has_setter;				// flag used to determine if the variable has a set property or not
	bool emit_create_cppobject_ctor;	// flag used to control the creation of createcppObjectcode for only once [per class]

	Hash *undefined_types;			// A hash table of types that we have seen but which may not have been defined. The index is a SwigType.
	
	const String *empty_string;		// empty string used at different places in the code
	const String *null_string;		// null string used at different places in the code
	
public:
	JAVASCRIPT() : member_variable_flag(false), static_member_variable_flag(false), global_variable_flag(false), static_member_func_flag(false), member_func_flag(false),  member_constant_flag(false), has_setter(false), emit_create_cppobject_ctor(false), undefined_types(NULL), empty_string(NewString("")), null_string(NewString("NULL")) {}
	~JAVASCRIPT() {}

	/* main and top */
    virtual void main(int argc, char *argv[]);
    virtual int top(Node *n);

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

	/* Code generators */
	void emitTypeWrapperClass(String *classname, SwigType *type);
	void emitAsCVARProperty(Node *n);
	void emitGetterSetter(Node *n, String *wname, ParmList *parms, Wrapper *wrapper, bool is_getter);
	//void emitcreatecppObjectCode(Node *n, String *wname, ParmList *parms, Wrapper *wrapper);
	void emitcreatecppObjectCode(Node *n, String *wname, ParmList *parms);
	
	/* Misc functions */
	void marshalInputArgs(ParmList *parms, int numarg, Wrapper *wrapper);
	void marshalOutput(Node *n, String *actioncode, Wrapper *wrapper);	
	String *getOverloadedName(Node *n);
	Parm *skipIgnoredArgs(Parm *p);
	//String *resolveType(SwigType *type);
	const String *typemapLookup(Node *n, const_String_or_char_ptr tmap_method, SwigType *type, int warning, Node *typemap_attributes = 0);
};


/* ---------------------------------------------------------------------
 * getOverloadedName()
 * All JSCORE wrapped methods have the same signature but with different names
 * To handle overloaded methods (which have the same name as well), the methods 
 * are mangled when overloaded to give them a unique name.
 * --------------------------------------------------------------------- */
String *JAVASCRIPT::getOverloadedName(Node *n) {
    String *overloaded_name = NewStringf("%s", Getattr(n, "sym:name"));
    if (Getattr(n, "sym:overloaded")) {
      Printv(overloaded_name, Getattr(n, "sym:overname"), NIL);
    }
    return overloaded_name;
 }

/* ---------------------------------------------------------------------
 * skipIgnoredArgs()
 *
 * --------------------------------------------------------------------- */
Parm *JAVASCRIPT::skipIgnoredArgs(Parm *p) {
    while (checkAttribute(p, "tmap:in:numinputs", "0")) {
      p = Getattr(p, "tmap:in:next");
    }
    return p;
}

/* ---------------------------------------------------------------------
 * resolveType()
 *
 * --------------------------------------------------------------------- */

/*String *JAVASCRIPT::resolveType(SwigType *t) {
    return null_string;
} */

/* ---------------------------------------------------------------------
 * marshalInputArgs()
 *
 * Process all of the arguments passed into the argv array
 * and convert them into C/C++ function arguments using the
 * supplied typemaps.
 * --------------------------------------------------------------------- */

void JAVASCRIPT::marshalInputArgs(ParmList *parms, int numarg, Wrapper *wrapper) {
	String *tm;
    Parm *p;
 	int i = 0;

	int start = (member_variable_flag || member_func_flag) ? 1 : 0;
    for (i = 0, p = parms; i < numarg; i++)
	{
		p = skipIgnoredArgs(p);
		SwigType *pt = Getattr(p, "type");

	   	String *arg = NewString("");
		if (i == 0) {
			Printv(arg, (start == 0) ? "argv[0]" : "", NIL);
			if(start) {
				p = Getattr(p, "tmap:in:next");
				Delete(arg);	
				continue;		// special case: skip the typemaps for the first argument
			}		
		} else {
			Printf(arg, "argv[%d]", i - start);
		}

		if ((tm = Getattr(p, "tmap:in")))     	// Get typemap for this argument
		{
			Replaceall(tm, "$input", arg);
			Setattr(p, "emit:input", arg);
			Printf(wrapper->code, "%s\n", tm);
			p = Getattr(p, "tmap:in:next");
      	} else {
		Swig_warning(WARN_TYPEMAP_IN_UNDEF, input_file, line_number, "Unable to use type %s as a function argument.\n", SwigType_str(pt, 0));
		p = nextSibling(p);
      	}
      Delete(arg);
    } 
}

/* ---------------------------------------------------------------------
 * marshalOutput()
 *
 * Process the return value of the C/C++ function call
 * and convert them into the Javascript types using the
 * supplied typemaps.
 * --------------------------------------------------------------------- */

void JAVASCRIPT::marshalOutput(Node *n, String *actioncode, Wrapper *wrapper) {
	SwigType *type = Getattr(n, "type");
	Setattr(n, "type", type);
	String *tm;
	if ((tm = Swig_typemap_lookup_out("out", n, "result", wrapper, actioncode))) 
	{
		Replaceall(tm, "$result", "jsresult");
		// TODO: May not be the correct way
		Replaceall(tm, "$objecttype", Swig_scopename_last(SwigType_str(SwigType_strip_qualifiers(type), 0)));
		Printf(wrapper->code, "%s", tm);
		if (Len(tm))
		  Printf(wrapper->code, "\n");
      } else {
		Swig_warning(WARN_TYPEMAP_OUT_UNDEF, input_file, line_number, "Unable to use return type %s in function %s.\n", SwigType_str(type, 0), Getattr(n, "name"));
      }
      emit_return_variable(n, type, wrapper);
}

/* -----------------------------------------------------------------------------
 * typemapLookup()
 * n - for input only and must contain info for Getfile(n) and Getline(n) to work
 * tmap_method - typemap method name
 * type - typemap type to lookup
 * warning - warning number to issue if no typemaps found
 * typemap_attributes - the typemap attributes are attached to this node and will 
 *   also be used for temporary storage if non null
 * return is never NULL, unlike Swig_typemap_lookup()
 * ----------------------------------------------------------------------------- */

const String *JAVASCRIPT::typemapLookup(Node *n, const_String_or_char_ptr tmap_method, SwigType *type, int warning, Node *typemap_attributes) {
	Node *node = !typemap_attributes ? NewHash() : typemap_attributes;
    Setattr(node, "type", type);
    Setfile(node, Getfile(n));
    Setline(node, Getline(n));
    const String *tm = Swig_typemap_lookup(tmap_method, node, "", 0);
    if (!tm) {
      tm = empty_string;
      if (warning != WARN_NONE)
	Swig_warning(warning, Getfile(n), Getline(n), "No %s typemap defined for %s\n", tmap_method, SwigType_str(type, 0));
    }
    if (!typemap_attributes)
      Delete(node);
    return tm;
  }

/* -----------------------------------------------------------------------------
 * emitTypeWrapperClass()
 * ----------------------------------------------------------------------------- */

void JAVASCRIPT::emitTypeWrapperClass(SwigType *type, String *classname) {
	Node *n = NewHash();
    Setfile(n, input_file);
    Setline(n, line_number);
	    
	/* Add the declaration & definition of access functions for this undefined type. */
	
	// Declaration
	// constructor for the JS object
	Printv(wrap_h_code, typemapLookup(n, "accessconstructordecl", classname,  WARN_CSHARP_TYPEMAP_CTYPE_UNDEF), ";\n", NIL);
	
	// access functions for the JS object
	Printv(wrap_h_code, typemapLookup(n, "accessfunctionsdecl", classname,  WARN_CSHARP_TYPEMAP_CTYPE_UNDEF), "\n", NIL);
	
	// destructor
	Printv(wrap_h_code, typemapLookup(n, "accessdestructordecl", classname,  WARN_CSHARP_TYPEMAP_CTYPE_UNDEF), ";\n", NIL);
	
	// Definition
	// constructor definition
	Printv(f_wrappers, typemapLookup(n, "accessconstructordecl", classname,  WARN_CSHARP_TYPEMAP_CTYPE_UNDEF), 
		   "\n", typemapLookup(n, "accessconstructorbody", classname,  WARN_CSHARP_TYPEMAP_CTYPE_UNDEF), "\n", NIL);
	
	// destructor definition
	Printv(f_wrappers, typemapLookup(n, "accessdestructordecl", classname,  WARN_CSHARP_TYPEMAP_CTYPE_UNDEF), 
		   "\n", typemapLookup(n, "accessdestructorbody", classname,  WARN_CSHARP_TYPEMAP_CTYPE_UNDEF), NIL);
	
	// the helper variables 
	Printv(f_wrappers, typemapLookup(n, "accessvariablesdefn", classname,  WARN_CSHARP_TYPEMAP_CTYPE_UNDEF), NIL);

	// definition of access functions for the JS object
	Printv(f_wrappers, typemapLookup(n, "accessfunctionsdefn", classname,  WARN_CSHARP_TYPEMAP_CTYPE_UNDEF), NIL);
	
	// create the cpp object code
	Clear(js_create_cpp_object_code);
	// TODO: emitcreatecppObjectCode(n, classname, NIL);
	
	// apply the necessary substitutions
	Replaceall(f_wrappers, "$objecttype", SwigType_str(type, 0));

	Replaceall(f_wrappers, "$jsstaticvaluescode", js_static_values_code);
	Replaceall(f_wrappers, "$jsstaticfuncscode", js_static_funcs_code);
	Replaceall(f_wrappers, "$jscreatecppobjectcode", js_create_cpp_object_code);	
	
	Replaceall(wrap_h_code, "$jsclassname", classname);
	Replaceall(f_wrappers, "$jsclassname", classname);
	Replaceall(wrap_h_code, "$classname", classname);
	Replaceall(f_wrappers, "$classname", classname);
	
	// Cleanup
    Delete(n);
}


/* -----------------------------------------------------------------------------
 * emitAsCVARProperty()
 * The function to emit global variables and class static member variables as 
 * properties of the cvar object
 * ----------------------------------------------------------------------------- */

void JAVASCRIPT::emitAsCVARProperty(Node *n) {
	String *name = Getattr(n, "name");
	String *propertyname;

	if(static_member_variable_flag) propertyname = NewStringf("%s_%s", current_class_name, name);
	else propertyname = NewStringf("%s", name);
	
	String *fname = NewStringf("_wrap_%s", propertyname);	
	String *getname = Swig_name_get(getNSpace(), fname);
	String *setname = Swig_name_set(getNSpace(), fname);
	
	static bool cvarclass = true;
	
	if(cvarclass) {
		Printv(wrap_h_code, typemapLookup(n, "cvardecl", name,  WARN_CSHARP_TYPEMAP_CTYPE_UNDEF), "\n", NIL);
		Printv(f_wrappers, typemapLookup(n, "cvardefn", name,  WARN_CSHARP_TYPEMAP_CTYPE_UNDEF), "\n", NIL);
		Clear(js_static_cvar_code);
		cvarclass = false;
	}

	// Add the entry for this variable to the staticvalues field of JSClassDefinition for cvar
	if(has_setter) {
		Printv(js_static_cvar_code, tab4, "{", "\"", propertyname,  "\",", getname, ",", setname, ", kJSPropertyAttributeNone },\n", NIL);
	}
	else {
		Printv(js_static_cvar_code, tab4, "{", "\"", propertyname,  "\",", getname, ",", "NULL", ", kJSPropertyAttributeNone },\n", NIL);
	}
	
	// Tidy up
	Delete(getname);
	Delete(setname);
	Delete(fname);
}


/* -----------------------------------------------------------------------------
 * emitGetterSetter()
 * The function to emit getter and setter functions
 * ----------------------------------------------------------------------------- */

void JAVASCRIPT::emitGetterSetter(Node *n, String *wname, ParmList *parms, Wrapper *wrapper, bool is_getter) {
	// Add any local variables
  	Wrapper_add_local(wrapper, "jsresult", "JSValueRef jsresult");	

	// Write the required arguments
   	emit_parameter_variables(parms, wrapper);

	// Attach the standard typemaps
	emit_attach_parmmaps(parms, wrapper);   
	Setattr(n, "wrap:parms", parms);
    
	// Get number of arguments
   	int numarg = emit_num_arguments(parms);
   	//int numreq = emit_num_required(parms);

	if(!is_getter) {  // marshal input argument
		String *tm;
		Parm *p;
		int i = 0;
    	for (i = 0, p = parms; i < numarg; i++)
		{
			p = skipIgnoredArgs(p);
			SwigType *pt = Getattr(p, "type");
			if (i == 0 && member_variable_flag) {
				p = Getattr(p, "tmap:in:next");				
				continue;	// special case: skip the typemaps for the first argument
			}
	     	if ((tm = Getattr(p, "tmap:in")))     	// Get typemap for this argument
			{
				Replaceall(tm, "$input", "value");
				Setattr(p, "emit:input", "value");
				Printf(wrapper->code, "%s\n", tm);
	      	} else {
				Swig_warning(WARN_TYPEMAP_IN_UNDEF, input_file, line_number, "Unable to use type %s as a function argument.\n", SwigType_str(pt, 0));
				p = nextSibling(p);
	      	}
	     } 
	}

	// Write constraints  

   	// Emit the action
	Setattr(n, "wrap:name", wname);
	String *actioncode = emit_action(n); 

	// Write typemaps(out)
   	marshalOutput(n, actioncode, wrapper);
}


/* ---------------------------------------------------------------------
 * emitcreatecppObjectCode()
 *
 * 
 * --------------------------------------------------------------------- */

/*void JAVASCRIPT::emitcreatecppObjectCode(Node *n, String *wname, ParmList *parms, Wrapper *wrapper) {
	SwigType *type = Getattr(n, "type");
	Printv(wrapper->def, "\nJSObjectRef $jsclassname_createcppObject(JSContextRef context, JSObjectRef object, size_t argc,",
	"const JSValueRef argv[], JSValueRef* exception) {", NIL);

	// Add any local variables
  	Wrapper_add_local(wrapper, "jsresult", "JSObjectRef jsresult");	
	Wrapper_add_local(wrapper, "result", "$type result = 0;");

	// Write the required arguments
   	emit_parameter_variables(parms, wrapper);

	// Attach the standard typemaps
	emit_attach_parmmaps(parms, wrapper);   
	Setattr(n, "wrap:parms", parms);
    
	// Get number of arguments
   	int numarg = emit_num_arguments(parms);
   	int numreq = emit_num_required(parms);

	// Check arguments
	Printf(wrapper->code, "if ((argc < %d) || (argc > %d)) ", numreq, numarg);
   	Printf(wrapper->code, "; //TODO: Raise exception for insuffucient number of arguments \n");

 	// Now walk the function parameter list and generate code to get arguments
	String *tm;
    Parm *p;
 	int i = 0;
	for (p = parms; p; p = nextSibling(p), i++)
	{
		SwigType *pt = Getattr(p, "type");
    	String *arg = NewString("");
		Printf(arg, "argv[%d]", i);
     	if ((tm = Getattr(p, "tmap:in")))     	// Get typemap for this argument
		{
			Replaceall(tm, "$input", arg);
			Setattr(p, "emit:input", arg);
			Printf(wrapper->code, "%s\n", tm);
      	} else {
		Swig_warning(WARN_TYPEMAP_IN_UNDEF, input_file, line_number, "Unable to use type %s as a function argument.\n", SwigType_str(pt, 0));
		p = nextSibling(p);
      	}
      Delete(arg);
    } 

   	// Emit the function call, out typemap and return statement
	Setattr(n, "wrap:name", wname);
	String *actioncode = emit_action(n); 

   	//marshalOutput(n, actioncode, wrapper);
	Printv(wrapper->code, actioncode, "jsresult = JSObjectMake(context, $jsclassname_GetJSClass(context), result);\n", NIL);
	Printv(wrapper->code, "return jsresult;\n}\n", NIL);
	Replaceall(wrapper->locals, "$type", SwigType_str(type, 0));
} */

void JAVASCRIPT::emitcreatecppObjectCode(Node *n, String *wname, ParmList *parms) {
	//SwigType *type = Getattr(n, "type");

	// Create a new wrapper object
    Wrapper *wrapper = NewWrapper();

	// Write the required arguments
   	emit_parameter_variables(parms, wrapper);

	// Attach the standard typemaps
	emit_attach_parmmaps(parms, wrapper);   
	Setattr(n, "wrap:parms", parms);

	// Get number of arguments
   	//int numarg = emit_num_arguments(parms);
   	int numreq = emit_num_required(parms);

	// Javascript doesnot support overloaded constructors based on types
	// Those with different number of arguments are handled by a simple if check on number of arguments passe
	Printf(js_create_cpp_object_code, "if (argc == %d) {\n", numreq);

	// Append locals to js_create_cpp_object_code
	Printv(js_create_cpp_object_code, wrapper->locals, NIL);

 	// Now walk the function parameter list and generate code to get arguments
	String *tm;
    Parm *p;
 	int i = 0;
	for (p = parms; p; p = nextSibling(p), i++)
	{
		SwigType *pt = Getattr(p, "type");
    	String *arg = NewString("");
		Printf(arg, "argv[%d]", i);
     	if ((tm = Getattr(p, "tmap:in")))     	// Get typemap for this argument
		{
			Replaceall(tm, "$input", arg);
			Setattr(p, "emit:input", arg);
			Printf(wrapper->code, "%s\n", tm);
      	} else {
		Swig_warning(WARN_TYPEMAP_IN_UNDEF, input_file, line_number, "Unable to use type %s as a function argument.\n", SwigType_str(pt, 0));
		p = nextSibling(p);
      	}
      Delete(arg);
    } 

   	// Emit the function call, out typemap and return statement
	Setattr(n, "wrap:name", wname);
	String *actioncode = emit_action(n); 

   	//marshalOutput(n, actioncode, wrapper);
	Printv(wrapper->code, tab4, actioncode, "\n", "}\n", NIL);
	
	// Append all to js_create_cpp_object_code
	Printv(js_create_cpp_object_code, wrapper->code,NIL);

	DelWrapper(wrapper);
}


/* ---------------------------------------------------------------------
 * main()
 *
 * Entry point for the javascript module
 * --------------------------------------------------------------------- */

void JAVASCRIPT::main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    // Set javascript subdirectory in SWIG library
    SWIG_library_directory("javascript");
    // Add a symbol to the parser for conditional compilation
    Preprocessor_define("SWIGJAVASCRIPT 1", 0);
    // Add typemap definitions
    SWIG_typemap_lang("javascript");
    // Set configuration file 
    SWIG_config_file("javascript.swg");
	// Allow overloading
	allow_overloading();
}

/* ---------------------------------------------------------------------
 * top()
 *
 * Function handler for processing top node of the parse tree 
 * Wrapper code generation essentially starts from here 
 * --------------------------------------------------------------------- */

int JAVASCRIPT::top(Node *n) {
    /* Get the module name */
    String *module = Getattr(n,"name");
    /* Get the output file name */
    String *outfile = Getattr(n,"outfile");

    /* Initialize I/O */
	String *outfile_h = Getattr(n, "outfile_h");
    f_wrap_h = NewFile(outfile_h, "w", SWIG_output_files());
    if (!f_wrap_h) {
       FileErrorDisplay(outfile);
       SWIG_exit(EXIT_FAILURE);
    }
    f_wrap_cpp = NewFile(outfile, "w", SWIG_output_files());
    if (!f_wrap_cpp) {
       FileErrorDisplay(outfile);
       SWIG_exit(EXIT_FAILURE);
    }

	/* Initialization of members */
    f_runtime = NewString("");
    f_init = NewString("");
    f_header = NewString("");
    f_wrappers = NewString("");

	wrap_h_code = NewString("");
	js_static_values_code = NewString("");
	js_static_funcs_code = NewString("");
	js_static_consts_code = NewString("");
	js_static_cvar_code = NewString("");
	js_create_cpp_object_code = NewString("");

	undefined_types = NewHash();
	
	/* Register file targets with the SWIG file handler */
    Swig_register_filebyname("begin", f_wrap_cpp);
    Swig_register_filebyname("header", f_header);
    Swig_register_filebyname("wrapper", f_wrappers);
    Swig_register_filebyname("runtime", f_runtime);
    Swig_register_filebyname("init", f_init);

    /* Output module initialization code */
    Swig_banner(f_wrap_h);
    Printf(f_wrap_h, "\n");
    Printf(f_wrap_h, "#ifndef %(upper)s_WRAP_H\n", module);
    Printf(f_wrap_h, "#define %(upper)s_WRAP_H\n\n", module);
	Printf(f_wrap_h, "\n#ifdef __cplusplus\n");
    Printf(f_wrap_h, "extern \"C\" {\n");
    Printf(f_wrap_h, "#endif\n\n");

	Swig_banner(f_wrap_cpp);
    Printf(f_header, "#include \"%s\"",outfile_h);

    /* Emit code for children */
    Language::top(n);

	// Write out definitions for the types not defined by SWIG.
    Printv(f_wrappers, "\n", NULL);
    for (Iterator p = First(undefined_types); p.key; p = Next(p)) {
		emitTypeWrapperClass(p.key, p.item);
    }

	
	// Substitutions
	Replaceall(f_wrappers, "$jsstaticconstscode", js_static_consts_code);
	Replaceall(f_wrappers, "$jsstaticvarcode", js_static_cvar_code);
	
	
    /* Write to the wrap_h */
    Dump(wrap_h_code, f_wrap_h);
    Printf(f_wrap_h, "#ifdef __cplusplus\n");
    Printf(f_wrap_h, "}\n");
    Printf(f_wrap_h, "#endif\n");
    Printf(f_wrap_h, "#endif\n\n");

    /* Write to the wrap_cpp */
    Dump(f_runtime, f_wrap_cpp);
    Dump(f_header, f_wrap_cpp);
    Dump(f_wrappers, f_wrap_cpp);
    Wrapper_pretty_print(f_init, f_wrap_cpp);

    /* Cleanup files and strings*/
	Delete(undefined_types);
	Delete(js_static_values_code);
	Delete(js_static_funcs_code);
	Delete(js_static_consts_code);
	Delete(js_static_cvar_code);
	Delete(js_create_cpp_object_code);
    Delete(f_runtime);
    Delete(f_header);
    Delete(f_wrappers);
    Delete(f_init);
	Delete(wrap_h_code);
    Close(f_wrap_h);
    Delete(f_wrap_h);
    Close(f_wrap_cpp);
    Delete(f_wrap_cpp);
    return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * constantWrapper()
 *
 * Low level code generator for constants 
 * --------------------------------------------------------------------- */

int JAVASCRIPT::constantWrapper(Node *n) {
  	SwigType *type = Getattr(n, "type");
	String *symname = Getattr(n, "sym:name");
	String *name = Getattr(n, "name");
	String *getname = Swig_name_get(getNSpace(), symname);
	String *tm;
	String *value = Getattr(n, "value");
	String *newvalue = NewString("");
    //bool is_enum_item = (Cmp(nodeType(n), "enumitem") == 0);

	static bool cconstclass = true;

	if(cconstclass) {
		Printv(wrap_h_code, typemapLookup(n, "cconstdecl", name,  WARN_CSHARP_TYPEMAP_CTYPE_UNDEF), "\n", NIL);
		Printv(f_wrappers, typemapLookup(n, "cconstdefn", name,  WARN_CSHARP_TYPEMAP_CTYPE_UNDEF), "\n", NIL);
		Clear(js_static_consts_code);
		cconstclass = false;
    }

	// Declaration of getter for this constant
	Printv(wrap_h_code, typemapLookup(n, "constantdecl", name,  WARN_CSHARP_TYPEMAP_CTYPE_UNDEF), ";\n", NIL);

	// Definition of getter for this constant	
	Printv(f_wrappers, typemapLookup(n, "constantdecl", name,  WARN_CSHARP_TYPEMAP_CTYPE_UNDEF), "\n{", NIL);
	Printv(f_wrappers, typemapLookup(n, "constantbody", name,  WARN_CSHARP_TYPEMAP_CTYPE_UNDEF), NIL);

	
	if (SwigType_type(type) == T_STRING) {
		Printv(newvalue, "\"", value, "\"", NIL);
    } else if (SwigType_type(type) == T_CHAR) {
		Printv(newvalue, "'", value, "'", NIL);
    } else {
		Printv(newvalue, value, NIL);
    }
	
	if ((tm = Swig_typemap_lookup("out", n, newvalue, 0))) 
	{
		Replaceall(tm, "$result", "jsresult");
		Printv(f_wrappers, tab4, tm, NIL);
		if (Len(tm))
		  Printf(f_wrappers, "\n");
      } else {
		Swig_warning(WARN_TYPEMAP_OUT_UNDEF, input_file, line_number, "Unable to use return type %s in function %s.\n", SwigType_str(type, 0), name);
    }

	Printv(f_wrappers, tab4, "return jsresult;\n}\n", NIL);

	
	// Substitutions
	Replaceall(wrap_h_code, "$constantgetname", getname);
	Replaceall(f_wrappers, "$constantgetname", getname);
	Replaceall(f_wrappers, "$objecttype", SwigType_str(type, 0));
	
	// Add the entry for this constant to the staticvalues field of JSClassDefinition
	Printv(js_static_consts_code, tab4, "{", "\"", symname, "\",", getname, ", NULL, kJSPropertyAttributeNone },\n", NIL);

	// Tidy up
	Delete(getname);
	Delete(newvalue);
	return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * variableWrapper()
 *
 * Low level code generator for variables  
 * --------------------------------------------------------------------- */

int JAVASCRIPT::variableWrapper(Node *n) {
	Language::variableWrapper(n);
	return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * nativeWrapper()
 *
 * 
 * --------------------------------------------------------------------- */

int JAVASCRIPT::nativeWrapper(Node *n) {
	Language::nativeWrapper(n);
	return SWIG_OK;
}


/* ---------------------------------------------------------------------
 * functionWrapper()
 *
 * Low level code generator for functions 
 * --------------------------------------------------------------------- */

int JAVASCRIPT::functionWrapper(Node *n) {
	// Get some useful attributes of this function
   	//String   *symname   = Getattr(n,"sym:name");
  	SwigType *type   = Getattr(n, "type");
   	ParmList *parms  = Getattr(n,"parms");
   	//String   *action = Getattr(n,"wrap:action");
	String 	 *name	 = Getattr(n,"name");
    bool constructor = (Cmp(nodeType(n), "constructor") == 0);
	bool destructor = (Cmp(nodeType(n), "destructor") == 0);
	bool is_getter = false;
	
    String *value = Getattr(n, "value");
    String *storage = Getattr(n, "storage");
    bool pure_virtual = false;
    if (Cmp(storage, "virtual") == 0) {
      if (Cmp(value, "0") == 0) {
			pure_virtual = true;
      }
    }

	// Do nothing for pure virtual functions
	if(pure_virtual) return SWIG_OK;


	// Overloaded name [will return a mangled name if the function is overloaded]
    String *overname = getOverloadedName(n);

   	// Create the wrapper function object
   	Wrapper *wrapper = NewWrapper();

	// Create the functions wrappered name
   	String *wname = Swig_name_wrapper(overname);

	/* First, write the function declaration to the wrapper header file. Nothing for constructor and destructor*/
	if (constructor) {
		if(emit_create_cppobject_ctor) {
			// We add our own constructor
			Printv(wrap_h_code, typemapLookup(n, "accessconstructordecl", name,  WARN_CSHARP_TYPEMAP_CTYPE_UNDEF), ";\n", NIL);
			// Also add access functions for JS objects
			Printv(wrap_h_code, typemapLookup(n, "accessfunctionsdecl", name,  WARN_CSHARP_TYPEMAP_CTYPE_UNDEF), "\n", NIL);
		}
	} else if (destructor) {
		Printv(wrap_h_code, typemapLookup(n, "accessdestructordecl", name,  WARN_CSHARP_TYPEMAP_CTYPE_UNDEF), ";\n", NIL);
	} else if(member_variable_flag || static_member_variable_flag || global_variable_flag) {
		// Handle getter & setter functions
		is_getter = (Cmp(wname, Swig_name_set(getNSpace(), variable_name)) != 0);	
		
		// Declaration and Definition
		if(is_getter) {
			Printv(wrap_h_code, typemapLookup(n, "getpropertydecl", name,  WARN_CSHARP_TYPEMAP_CTYPE_UNDEF), ";\n", NIL);
			Printv(wrapper->def, typemapLookup(n, "getpropertydecl", name,  WARN_CSHARP_TYPEMAP_CTYPE_UNDEF), "\n", "{", NIL);
		} else {
			has_setter = true;			
			Printv(wrap_h_code, typemapLookup(n, "setpropertydecl", name,  WARN_CSHARP_TYPEMAP_CTYPE_UNDEF), ";\n", NIL);
			Printv(wrapper->def, typemapLookup(n, "setpropertydecl", name,  WARN_CSHARP_TYPEMAP_CTYPE_UNDEF), "\n", "{", NIL);
		}
		
		emitGetterSetter(n, wname, parms, wrapper, is_getter);

		// Close the definition
	    Printv(wrapper->code, tab4, "return jsresult;\n", NIL);
	    Append(wrapper->code, "}\n");
		
		// Dump the functions out
   		Wrapper_print(wrapper,f_wrappers);

		// Substititions
		Replaceall(wrap_h_code, "$getname", wname);
		Replaceall(wrap_h_code, "$setname", wname);
		Replaceall(f_wrappers, "$getname", wname);
		Replaceall(f_wrappers, "$setname", wname);

		// Tidy up, and return 
		Delete(overname);
  		Delete(wname);
    	DelWrapper(wrapper);
    	return SWIG_OK;

	}
	else {
		Printv(wrap_h_code, typemapLookup(n, "functiondecl", name,  WARN_CSHARP_TYPEMAP_CTYPE_UNDEF), ";\n", NIL);
		Replaceall(wrap_h_code, "$functionname", wname);
	}
	
	/* Now write the wrap_cpp */
	if(constructor) {	// Write the constructor and the required access functions here
		if(emit_create_cppobject_ctor) { 
			// Definition of access functions
			Printv(wrapper->code, typemapLookup(n, "accessfunctionsdefn", name,  WARN_CSHARP_TYPEMAP_CTYPE_UNDEF), NIL);

			// Constructor definition
	  		Printv(wrapper->code, typemapLookup(n, "accessconstructordecl", name,  WARN_CSHARP_TYPEMAP_CTYPE_UNDEF), 
			"\n", typemapLookup(n, "accessconstructorbody", name,  WARN_CSHARP_TYPEMAP_CTYPE_UNDEF), "\n", NIL);

			// substitutions
			Replaceall(wrapper->code, "$objecttype", SwigType_str(type, 0));

			// Dump the function out	
			Wrapper_print(wrapper,f_wrappers);

			// reset the flag to prevent this code being generated again for overloaded constructors
			emit_create_cppobject_ctor = false;
		}
		emitcreatecppObjectCode(n, wname, parms);
	
		// Tidy up and return
		Delete(overname);
   		Delete(wname);
    	DelWrapper(wrapper);
    	return SWIG_OK;
	}
	else if (destructor) {
		Printv(wrapper->code, typemapLookup(n, "accessdestructordecl", name,  WARN_CSHARP_TYPEMAP_CTYPE_UNDEF), 
		"\n", typemapLookup(n, "accessdestructorbody", name,  WARN_CSHARP_TYPEMAP_CTYPE_UNDEF), NIL);

		// substitutions
		Replaceall(wrapper->code, "$objecttype", SwigType_str(current_class_type, 0));
		
		// Dump the function out, Tidy up and return
   		Wrapper_print(wrapper,f_wrappers);
		Delete(overname);
   		Delete(wname);
    	DelWrapper(wrapper);
    	return SWIG_OK;
	}

	/* else handle Global functions, class member functions, global variables, member variables and static members */

	// Add the function entry to the staticfunctions field of JSClassDefinition of the class object
	// global variables are only added as property to the cvar object, and static members as properties to both (in the handler function)
	if(!global_variable_flag) {
		Printv(js_static_funcs_code, tab4, "{", "\"", Swig_scopename_last(name), "\",", wname, ",kJSPropertyAttributeDontDelete },\n", NIL);
	}

	// Function definition
	Printv(wrapper->def, typemapLookup(n, "functiondecl", name,  WARN_CSHARP_TYPEMAP_CTYPE_UNDEF),
	"\n", "{", NIL);
	Replaceall(wrapper->def, "$functionname", wname);

   	// If any additional local variable needed, add them now
   	Wrapper_add_local(wrapper, "jsresult", "JSValueRef jsresult");	

	// Write the required arguments
   	emit_parameter_variables(parms, wrapper);

	// Attach the standard typemaps
	emit_attach_parmmaps(parms, wrapper);   
	Setattr(n, "wrap:parms", parms);
    
	// Get number of arguments
   	int numarg = emit_num_arguments(parms);
   	int numreq = emit_num_required(parms);
    //printf("numarg=%d and numreq=%d\n", numarg, numreq);

	// TODO: Add code to check arguments.
	//Printf(wrapper->code, "if ((argc < %d) || (argc > %d)) ", numreq, numarg);
   	//Printf(wrapper->code, ";/* TODO: Raise exception for insuffucient number of arguments*/\n");

 	// Now walk the function parameter list and generate code to get arguments
	marshalInputArgs(parms, numarg, wrapper);

	// Write constraints  

   	// Emit the function call
	Setattr(n, "wrap:name", wname);
	String *actioncode = emit_action(n); 

	// Write typemaps(out)
   	marshalOutput(n, actioncode, wrapper);
 
	// Add cleanup code

	/* Close the function */
	Printv(wrapper->code, "return jsresult;\n}", NIL);

   	/* final substititions if applicable */
	Replaceall(f_wrappers, "$objecttype", SwigType_str(type, 0));

   	/* Dump the function out */
   	Wrapper_print(wrapper,f_wrappers);

    /* Tidy up */
	Delete(overname);
    Delete(wname);
    DelWrapper(wrapper);
    return SWIG_OK;
}


/* ---------------------------------------------------------------------
 * functionHandler()
 *
 * Function handler for generating wrappers for functions 
 * --------------------------------------------------------------------- */

int JAVASCRIPT::functionHandler(Node *n) {
	Language::functionHandler(n);
	return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * globalfunctionHandler()
 *
 * Function handler for generating wrappers for functions 
 * --------------------------------------------------------------------- */

int JAVASCRIPT::globalfunctionHandler(Node *n) {
	Language::globalfunctionHandler(n);
 	return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * memberfunctionHandler()
 *
 * Function handler for generating wrappers for functions 
 * --------------------------------------------------------------------- */

int JAVASCRIPT::memberfunctionHandler(Node *n) {
	member_func_flag = true;
	Language::memberfunctionHandler(n);
	member_func_flag = false;
 	return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * staticmemberfunctionHandler()
 *
 * Function handler for generating wrappers for static member functions 
 * --------------------------------------------------------------------- */

int JAVASCRIPT::staticmemberfunctionHandler(Node *n) {
	static_member_func_flag = true;
	Language::staticmemberfunctionHandler(n);
	static_member_func_flag = false;
	return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * callbackfunctionHandler()
 *
 * Function handler for generating wrappers for callbacks 
 * --------------------------------------------------------------------- */

int JAVASCRIPT::callbackfunctionHandler(Node *n) {
	Language::callbackfunctionHandler(n);
 	return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * variableHandler()
 *
 * Function handler for generating wrappers for variables 
 * --------------------------------------------------------------------- */

int JAVASCRIPT::variableHandler(Node *n) {	
	Language::variableHandler(n);
 	return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * globalvariableHandler()
 *
 * Function handler for generating wrappers for global variables 
 * --------------------------------------------------------------------- */

int JAVASCRIPT::globalvariableHandler(Node *n) {
	String *symname = Getattr(n, "sym:name");
	String *fname = NewStringf("_wrap_%s", symname);
	
	
//  // Code to access Global variables as global functions
//	variable_name = Getattr(n, "sym:name");
//	global_variable_flag = true;
//	Language::globalvariableHandler(n);
//	global_variable_flag = false;
//	return SWIG_OK;
		
	variable_name = fname;
	global_variable_flag = true;
	has_setter = false;
	Language::globalvariableHandler(n);
	
	// emit this variable as property of the cvar object
	emitAsCVARProperty(n);
	global_variable_flag = false;
	
	Delete(fname);
	return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * membervariableHandler()
 *
 * Function handler for generating wrappers for member variables 
 * --------------------------------------------------------------------- */

int JAVASCRIPT::membervariableHandler(Node *n) {
	String *symname = Getattr(n, "sym:name");
	String *fname = NewStringf("_wrap_%s_%s", current_class_name, symname);
	String *getname = Swig_name_get(getNSpace(), fname);
	String *setname = Swig_name_set(getNSpace(), fname);

	variable_name = fname;
	member_variable_flag = true;

	// Add the property entry to the staticvalues field of JSClassDefinition
	Printv(js_static_values_code, tab4, "{", "\"", symname, "\",", getname, ",", setname, ",kJSPropertyAttributeNone },\n", NIL);

	Language::membervariableHandler(n);
	member_variable_flag = false;
	
	Delete(fname);
	Delete(getname);
	Delete(setname);	
 	return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * staticmembervariableHandler()
 *
 * Function handler for generating wrappers for static member variables 
 * --------------------------------------------------------------------- */

int JAVASCRIPT::staticmembervariableHandler(Node *n) {
	String *symname = Getattr(n, "sym:name");
	String *fname = NewStringf("_wrap_%s_%s", current_class_name, symname);
		
	static_member_variable_flag = true;
	has_setter = false;
	variable_name = fname;
	Language::staticmembervariableHandler(n);
  
	// emit this variable as property of the cvar object
	emitAsCVARProperty(n);
	static_member_variable_flag = false;
	
	Delete(fname);
 	return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * memberconstantHandler()
 *
 * Function handler for generating wrappers for member constant 
 * --------------------------------------------------------------------- */

int JAVASCRIPT::memberconstantHandler(Node *n) {
	member_constant_flag = true;
	Language::memberconstantHandler(n);
	member_constant_flag = false;
 	return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * constructorHandler()
 *
 * Function handler for generating wrappers for constructor 
 * --------------------------------------------------------------------- */

int JAVASCRIPT::constructorHandler(Node *n) {	
	/* Handle children */
	Language::constructorHandler(n);

 	return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * copyconstructorHandler()
 *
 * Function handler for generating wrappers for copy constructor 
 * --------------------------------------------------------------------- */

int JAVASCRIPT::copyconstructorHandler(Node *n) {
	Language::copyconstructorHandler(n);
 	return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * destructorHandler()
 *
 * Function handler for generating wrappers for destructor
 * --------------------------------------------------------------------- */

int JAVASCRIPT::destructorHandler(Node *n) {
	Language::destructorHandler(n);
 	return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * classHandler()
 *
 * Function handler for generating wrappers for class 
 * --------------------------------------------------------------------- */

int JAVASCRIPT::classHandler(Node *n) {
	String *symname   = Getattr(n,"sym:name");
    String *name = Getattr(n, "name");
	String *jsclassname = NewStringf("_wrap_%s",Swig_scopename_last(symname));
	emit_create_cppobject_ctor = true;

	// Set the current_class_name String used by children for code generation
	current_class_name = Swig_scopename_last(name);
	current_class_type = name;
	
	// Clear strings for the new class
	Clear(js_static_values_code);
	Clear(js_static_funcs_code);
	Clear(js_create_cpp_object_code);

	// Write the helper variables to the wrapper source file
	Printv(f_wrappers, typemapLookup(n, "accessvariablesdefn", name,  WARN_CSHARP_TYPEMAP_CTYPE_UNDEF), NIL);
	
	// Retreive the staticvalues and staticfuncs from the base classes (if any)
	List *baselist = Getattr(n, "bases");
    if (baselist) {
		Iterator base = First(baselist);
        while (base.item && GetFlag(base.item, "feature:ignore")) {
          base = Next(base);
        }
       if (base.item) {
			String *baseclassname = SwigType_namestr(Getattr(base.item, "name"));
			Node *baseclass = Swig_symbol_clookup(baseclassname, 0);
			String *values_code = SwigType_namestr(Getattr(baseclass, "classHandler:values"));
			String *funcs_code = SwigType_namestr(Getattr(baseclass, "classHandler:funcs"));
			Printv(js_static_values_code, values_code, "\n", NIL);
			Printv(js_static_funcs_code, funcs_code, "\n", NIL);
        }
    }

	/* Handle children */
	Language::classHandler(n);

    // Store the static values and functions for this class (if any)
	String *values_code = NewStringf("%s", js_static_values_code);
	String *funcs_code = NewStringf("%s", js_static_funcs_code);
			
	Setattr(n, "classHandler:values", values_code);
	Setattr(n, "classHandler:funcs", funcs_code);


	// Apply the necessary substitutions
	Replaceall(f_wrappers, "$jsstaticvaluescode", js_static_values_code);
	Replaceall(f_wrappers, "$jsstaticfuncscode", js_static_funcs_code);
	Replaceall(f_wrappers, "$jscreatecppobjectcode", js_create_cpp_object_code);	
	
	Replaceall(wrap_h_code, "$jsclassname", jsclassname);
	Replaceall(f_wrappers, "$jsclassname", jsclassname);
	Replaceall(wrap_h_code, "$classname", current_class_name);
	Replaceall(f_wrappers, "$classname", current_class_name);
	
	Delete(jsclassname);
	Delete(values_code);
	Delete(funcs_code);
 	return SWIG_OK;
}


/* -----------------------------------------------------------------------------
 * swig_javascript()    - Instantiate module
 * ----------------------------------------------------------------------------- */

static Language *new_swig_javascript() {
  return new JAVASCRIPT();
}
extern "C" Language *swig_javascript(void) {
  return new_swig_javascript();
}
