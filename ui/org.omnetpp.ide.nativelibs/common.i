%module Common

// covariant return type warning disabled
#pragma SWIG nowarn=822

%{
#include "common/stringutil.h"
#include "common/patternmatcher.h"
#include "common/unitconversion.h"
#include "common/bigdecimal.h"
#include "common/rwlock.h"
#include "common/expression.h"
#include "common/stringtokenizer2.h"
#include "common/fileutil.h"

using namespace omnetpp::common;
%}

%include "defs.i"
%include "loadlib.i"
%include "pefileversion.i"
%include "std_string.i"
%include "std_vector.i"

namespace std {
   %template(StringVector) vector<string>;
   %template(PStringVector) vector<const char *>;
}

#define THREADED

// hide export/import macros from swig
#define COMMON_API
#define OPP_DLLEXPORT
#define OPP_DLLIMPORT

%include "std_set.i"     // our custom version
namespace std {
   %template(StringSet) set<string>;
};

namespace omnetpp { namespace common {

%rename(parseQuotedString)   opp_parsequotedstr;
%rename(quoteString)         opp_quotestr;
%rename(needsQuotes)         opp_needsquotes;
%rename(quoteStringIfNeeded) opp_quotestr_ifneeded;
%rename(formatDouble)        opp_formatdouble;

std::string opp_parsequotedstr(const char *txt);
std::string opp_quotestr(const std::string& txt);
bool opp_needsquotes(const char *txt);
std::string opp_quotestr_ifneeded(const std::string& txt);
int strdictcmp(const char *s1, const char *s2);
std::string opp_formatdouble(double value, int numSignificantDigits);
//int getPEVersion(const char *fileName);

%ignore UnitConversion::parseQuantity(const char *, std::string&);

typedef int64_t intpar_t;

} } // namespaces

%include "common/patternmatcher.h"
%include "common/unitconversion.h"


/* -------------------- BigDecimal -------------------------- */

namespace omnetpp { namespace common {

%{
// some shitty Windows header file defines min()/max() macros that conflict with us
#undef min
#undef max
%}

%ignore _I64_MAX_DIGITS;
%ignore BigDecimal::BigDecimal();
%ignore BigDecimal::str(char*);
%ignore BigDecimal::parse(const char*,const char*&);
%ignore BigDecimal::ttoa;
%ignore BigDecimal::Nil;
%ignore BigDecimal::isNil;
%ignore BigDecimal::operator=;
%ignore BigDecimal::operator+=;
%ignore BigDecimal::operator-=;
%ignore BigDecimal::operator*=;
%ignore BigDecimal::operator/=;
%ignore BigDecimal::operator!=;
%ignore operator+;
%ignore operator-;
%ignore operator*;
%ignore operator/;
%ignore operator<<;
%immutable BigDecimal::Zero;
%immutable BigDecimal::NaN;
%immutable BigDecimal::PositiveInfinity;
%immutable BigDecimal::NegativeInfinity;
%rename(equals) BigDecimal::operator==;
%rename(less) BigDecimal::operator<;
%rename(greater) BigDecimal::operator>;
%rename(lessOrEqual) BigDecimal::operator<=;
%rename(greaterOrEqual) BigDecimal::operator>=;
%rename(toString) BigDecimal::str;
%rename(doubleValue) BigDecimal::dbl;

/*
FIXME new swig errors:
/home/andras/omnetpp/src/common/bigdecimal.h:120: Warning 503: Can't wrap 'operator +' unless renamed to a valid identifier.
/home/andras/omnetpp/src/common/bigdecimal.h:121: Warning 503: Can't wrap 'operator -' unless renamed to a valid identifier.
/home/andras/omnetpp/src/common/bigdecimal.h:123: Warning 503: Can't wrap 'operator *' unless renamed to a valid identifier.
/home/andras/omnetpp/src/common/bigdecimal.h:124: Warning 503: Can't wrap 'operator *' unless renamed to a valid identifier.
/home/andras/omnetpp/src/common/bigdecimal.h:125: Warning 503: Can't wrap 'operator /' unless renamed to a valid identifier.
/home/andras/omnetpp/src/common/bigdecimal.h:126: Warning 503: Can't wrap 'operator /' unless renamed to a valid identifier.
*/

%extend BigDecimal {
   const BigDecimal add(const BigDecimal& x) {return *self + x;}
   const BigDecimal subtract(const BigDecimal& x) {return *self - x;}
}

SWIG_JAVABODY_METHODS(public, public, BigDecimal)

%typemap(javacode) BigDecimal %{
    public boolean equals(Object other) {
       return other instanceof BigDecimal && equals((BigDecimal)other);
    }

    public int hashCode() {
       return (int)getIntValue();
    }

    public java.math.BigDecimal toBigDecimal() {
       long intVal = getIntValue();
       int scale = getScale();
       java.math.BigDecimal d = new java.math.BigDecimal(intVal);
       return (scale == 0 ? d : d.movePointRight(scale));
    }
%}

} } // namespaces

const omnetpp::common::BigDecimal floor(const omnetpp::common::BigDecimal& x);
const omnetpp::common::BigDecimal ceil(const omnetpp::common::BigDecimal& x);
const omnetpp::common::BigDecimal fabs(const omnetpp::common::BigDecimal& x);
const omnetpp::common::BigDecimal fmod(const omnetpp::common::BigDecimal& x, const omnetpp::common::BigDecimal& y);
const omnetpp::common::BigDecimal max(const omnetpp::common::BigDecimal& x, const omnetpp::common::BigDecimal& y);
const omnetpp::common::BigDecimal min(const omnetpp::common::BigDecimal& x, const omnetpp::common::BigDecimal& y);

%include "common/bigdecimal.h"


/* -------------------- rwlock.h -------------------------- */

namespace omnetpp { namespace common {

%ignore ReaderMutex;
%ignore WriterMutex;
SWIG_JAVABODY_METHODS(public, public, ILock)

} } // namespaces

%include "common/rwlock.h"

/* -------------------- expression.h -------------------------- */

namespace omnetpp { namespace common {

%ignore Expression::AstNode;
%ignore Expression::AstTranslator;
%ignore Expression::MultiAstTranslator;
%ignore Expression::BasicAstTranslator;
%ignore Expression::installAstTranslator;
%ignore Expression::getInstalledAstTranslators;
%ignore Expression::getDefaultAstTranslator;
%ignore Expression::setExpressionTree;
%ignore Expression::getExpressionTree;
%ignore Expression::removeExpressionTree;
%ignore Expression::evaluate;
%ignore Expression::parseToAst;
%ignore Expression::translateToExpressionTree;
%ignore Expression::performConstantFolding;
%ignore Expression::parseAndTranslate;
%ignore Expression::dumpTree;
%ignore Expression::dumpAst;
%ignore Expression::dumpAst;
%ignore Expression::dumpExprTree;




} } // namespaces

%include "common/expression.h"

/* -------------------- stringtokenizer2.h -------------------------- */

namespace omnetpp { namespace common {

%define CHECK_STRINGTOKENIZER_EXCEPTION(METHOD)
%exception METHOD {
    try {
        $action
    } catch (StringTokenizerException& e) {
        jclass clazz = jenv->FindClass("org/omnetpp/common/engineext/StringTokenizerException");
        jmethodID methodId = jenv->GetMethodID(clazz, "<init>", "(Ljava/lang/String;)V");
        jthrowable exception = (jthrowable)(jenv->NewObject(clazz, methodId, jenv->NewStringUTF(e.what())));
        jenv->Throw(exception);
        return $null;
    } catch (std::exception& e) {
        SWIG_JavaThrowException(jenv, SWIG_JavaRuntimeException, const_cast<char*>(e.what()));
        return $null;
    }
}
%enddef

CHECK_STRINGTOKENIZER_EXCEPTION(StringTokenizer2::StringTokenizer2)
CHECK_STRINGTOKENIZER_EXCEPTION(StringTokenizer2::setParentheses)
CHECK_STRINGTOKENIZER_EXCEPTION(StringTokenizer2::nextToken)

%ignore StringTokenizerException;

} } // namespaces

%include "common/stringtokenizer2.h"

/* -------------------- fileutil.h -------------------------- */

namespace omnetpp { namespace common {

// only wrap the following ones, for other functions we have java.io.File
std::vector<std::string> collectFilesInDirectory(const char *foldername, bool deep, const char *suffix=nullptr);
std::vector<std::string> collectMatchingFiles(const char *globstarPattern);

} } // namespaces

