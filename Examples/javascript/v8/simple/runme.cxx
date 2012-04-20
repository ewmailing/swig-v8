#include "JavaScriptCore/JavaScript.h"
#include "example_wrap.h"
#include <cstdio>
#include <cstdlib>

static JSValueRef	print(
			JSContextRef		context, 
			JSObjectRef			object, 
			JSObjectRef			thisObject, 
			size_t				argumentCount, 
			const JSValueRef	arguments[], 
			JSValueRef*			exception)
{
	if (argumentCount > 0)
	{
		JSStringRef string = JSValueToStringCopy(context, arguments[0], NULL);
		size_t numChars = JSStringGetMaximumUTF8CStringSize(string);
		char stringUTF8[numChars];
		JSStringGetUTF8CString(string, stringUTF8, numChars);
		printf("%s\n", stringUTF8);
	}

	return JSValueMakeUndefined(context);
}


static char* createStringWithContentsOfFile(const char* fileName)
{
	char* buffer;
	
	size_t buffer_size = 0;
	size_t buffer_capacity = 1024;
	buffer = (char*)malloc(buffer_capacity);
	
	FILE* f = fopen(fileName, "r");
	if (!f)
	{
		fprintf(stderr, "Could not open file: %s\n", fileName);
		return 0;
	}
	
	while (!feof(f) && !ferror(f))
	{
		buffer_size += fread(buffer + buffer_size, 1, buffer_capacity - buffer_size, f);
		if (buffer_size == buffer_capacity)
		{ 
			// guarantees space for trailing '\0'
			buffer_capacity *= 2;
			buffer = (char*)realloc(buffer, buffer_capacity);
		}
	}
	fclose(f);
	buffer[buffer_size] = '\0';
	
	return buffer;
}


bool initFunction(JSGlobalContextRef rContext, JSObjectRef rObject, const char* szFunctionName,
                               JSObjectCallAsFunctionCallback cbFunction)
{
    JSStringRef	functionString = JSStringCreateWithUTF8CString(szFunctionName);
    JSObjectSetProperty(
			rContext,
			rObject,
			functionString,
			JSObjectMakeFunctionWithCallback(
				rContext,
				functionString,
				cbFunction),
			kJSPropertyAttributeNone,
			NULL);
	JSStringRelease(functionString);
	return true;
}


int main(int argc, char* argv[]) {

  JSGlobalContextRef context = JSGlobalContextCreate(NULL);
  JSObjectRef globalObject = JSContextGetGlobalObject(context);
  initFunction(context, globalObject, "print", print); // Utility print function

  // Define the cvar object to access Global Variables
  cvar_initClass(context, globalObject, "cvar");
	
  // Define the functions
  initFunction(context, globalObject, "gcdi", _wrap_gcd__SWIG_0); // overloaded
  initFunction(context, globalObject, "gcdf", _wrap_gcd__SWIG_1); // overloaded

  initFunction(context, globalObject, "bar3", _wrap_bar__SWIG_0); // default arguments expanded as overloaded
  initFunction(context, globalObject, "bar2", _wrap_bar__SWIG_1); // default arguments expanded as overloaded
  initFunction(context, globalObject, "bar1", _wrap_bar__SWIG_2); // default arguments expanded as overloaded

  initFunction(context, globalObject, "helloString0", _wrap_helloString__SWIG_0); 
  initFunction(context, globalObject, "delete_helloString",_wrap_delete_helloString);
  initFunction(context, globalObject, "helloString1", _wrap_helloString__SWIG_1);
	
  

 // Evaluate the javascript
  char*	szString = createStringWithContentsOfFile("runme.js");
  JSStringRef Script = JSStringCreateWithUTF8CString(szString);

  JSValueRef Exception;
  JSValueRef Result = JSEvaluateScript(context,Script,NULL,NULL,0,&Exception);

  if (Result) 
	printf("runme.js executed successfully\n");
		
  else {
	printf("Script Exception\n");
	JSStringRef ExceptionIString;
	ExceptionIString = JSValueToStringCopy(context, Exception, NULL);
	char stringUTF8[256];
	JSStringGetUTF8CString(ExceptionIString, stringUTF8, 256);
	printf(":%s\n",stringUTF8);
	JSStringRelease(ExceptionIString);
  }
  
  if (szString != NULL)
	free(szString);

  JSStringRelease(Script);
  globalObject = 0;
  JSGlobalContextRelease(context);

   return 0;
}
