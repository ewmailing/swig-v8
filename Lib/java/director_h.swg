/* -----------------------------------------------------------------------------
 * director_h.swg
 *
 * This file provides declarations to support director classes
 * which provide a means to call methods from C++ to Java extensions.
 * ----------------------------------------------------------------------------- */

#ifndef SWIG_JAVA_DIRECTOR_DECLARATIONS
#define SWIG_JAVA_DIRECTOR_DECLARATIONS

#ifdef __cplusplus

namespace Swig {
  /* Java object wrapper */
  class JObjectWrapper {

  public:
    JObjectWrapper();

    ~JObjectWrapper();

    bool set(JNIEnv *jenv, jobject jobj, bool mem_own, bool weak_global);

    jobject get(JNIEnv *jenv) const;

    void release(JNIEnv *jenv);

    jobject peek();

    void java_change_ownership(JNIEnv *jenv, jobject jself, bool take_or_release);

  private:
    /* pointer to Java object */
    jobject jthis_;
    /* Local or global reference flag */
    bool weak_global_;
  };

  /* director base class */
  class Director {
    /* pointer to Java virtual machine */
    JavaVM *swig_jvm_;

  protected:

#if defined (_MSC_VER) && (_MSC_VER<1300)
    class JNIEnvWrapper;
    friend class JNIEnvWrapper;
#endif

    /* Utility class for managing the JNI environment */
    class JNIEnvWrapper {
      const Director *director_;
      JNIEnv *jenv_;
      int env_status;
    public:
      JNIEnvWrapper(const Director *director);
      ~JNIEnvWrapper();
      JNIEnv *getJNIEnv() const;
    };

    /* Java object wrapper */
    JObjectWrapper swig_self_;

    /* Disconnect director from Java object */
    void swig_disconnect_director_self(const char *disconn_method);

  public:
    Director(JNIEnv *jenv);

    virtual ~Director();

    bool swig_set_self(JNIEnv *jenv, jobject jself, bool mem_own, bool weak_global);

    jobject swig_get_self(JNIEnv *jenv) const;

    // Change C++ object's ownership, relative to Java
    void swig_java_change_ownership(JNIEnv *jenv, jobject jself, bool take_or_release);
  };
}

#endif /* __cplusplus */

#endif /* SWIG_JAVA_DIRECTOR_DECLARATIONS */
