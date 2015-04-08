//==========================================================================
//   CSTRINGPAR.CC  - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "omnetpp/cstringparimpl.h"
#include "omnetpp/cdynamicexpression.h"
#include "omnetpp/ccomponent.h"
#include "common/stringutil.h"

NAMESPACE_BEGIN


cStringParImpl::cStringParImpl()
{
}

cStringParImpl::~cStringParImpl()
{
    deleteOld();
}

void cStringParImpl::copy(const cStringParImpl& other)
{
    if (flags & FL_ISEXPR)
        expr = (cExpression *) other.expr->dup();
    else
        val = other.val;
}

void cStringParImpl::operator=(const cStringParImpl& other)
{
    if (this==&other) return;
    deleteOld();
    cParImpl::operator=(other);
    copy(other);
}

void cStringParImpl::parsimPack(cCommBuffer *buffer)
{
    //TBD
}

void cStringParImpl::parsimUnpack(cCommBuffer *buffer)
{
    //TBD
}

void cStringParImpl::setBoolValue(bool b)
{
    throw cRuntimeError(this, E_BADCAST, "bool", "string");
}

void cStringParImpl::setLongValue(long l)
{
    throw cRuntimeError(this, E_BADCAST, "int/long", "string");
}

void cStringParImpl::setDoubleValue(double d)
{
    throw cRuntimeError(this, E_BADCAST, "double", "string");
}

void cStringParImpl::setStringValue(const char *s)
{
    deleteOld();
    val = (s ? s : "");
    flags |= FL_CONTAINSVALUE | FL_ISSET;
}

void cStringParImpl::setXMLValue(cXMLElement *node)
{
    throw cRuntimeError(this, E_BADCAST, "XML", "string");
}

void cStringParImpl::setExpression(cExpression *e)
{
    deleteOld();
    expr = e;
    flags |= FL_ISEXPR | FL_CONTAINSVALUE | FL_ISSET;
}

bool cStringParImpl::boolValue(cComponent *context) const
{
    throw cRuntimeError(this, E_BADCAST, "string", "bool");
}

long cStringParImpl::longValue(cComponent *) const
{
    throw cRuntimeError(this, E_BADCAST, "string", "int/long");
}

double cStringParImpl::doubleValue(cComponent *) const
{
    throw cRuntimeError(this, E_BADCAST, "string", "double");
}

const char *cStringParImpl::stringValue(cComponent *context) const
{
    if ((flags & FL_ISSET) == 0)
        throw cRuntimeError(E_PARNOTSET);

    if (flags & FL_ISEXPR)
        throw cRuntimeError(this, "stringValue() and conversion to `const char *' cannot be invoked "
                                  "on parameters declared `volatile string' in NED -- use stdstringValue() "
                                  "or conversion to `std::string' instead.");
    return val.c_str();
}

std::string cStringParImpl::stdstringValue(cComponent *context) const
{
    if ((flags & FL_ISSET) == 0)
        throw cRuntimeError(E_PARNOTSET);

    if ((flags & FL_ISEXPR) == 0)
        return val;
    else {
        cNEDValue v = evaluate(expr, context);
        if (v.type != cNEDValue::STR)
            throw cRuntimeError(E_ECANTCAST, "string");
        return v.stringValue();
    }
}

cXMLElement *cStringParImpl::xmlValue(cComponent *) const
{
    throw cRuntimeError(this, E_BADCAST, "string", "XML");
}

cExpression *cStringParImpl::getExpression() const
{
    return (flags | FL_ISEXPR) ? expr : NULL;
}

void cStringParImpl::deleteOld()
{
    if (flags & FL_ISEXPR)
    {
        delete expr;
        flags &= ~FL_ISEXPR;
    }
}

cPar::Type cStringParImpl::getType() const
{
    return cPar::STRING;
}

bool cStringParImpl::isNumeric() const
{
    return false;
}

void cStringParImpl::convertToConst(cComponent *context)
{
    setStringValue(stdstringValue(context).c_str());
}

std::string cStringParImpl::str() const
{
    if (flags & FL_ISEXPR)
        return expr->str();

    return opp_quotestr(val.c_str());
}

void cStringParImpl::parse(const char *text)
{
    // try parsing it as an expression
    cDynamicExpression *dynexpr = new cDynamicExpression();
    try
    {
        dynexpr->parse(text);
    }
    catch (std::exception& e)
    {
        delete dynexpr;
        throw;
    }
    setExpression(dynexpr);

    // simplify if possible: store as constant instead of expression
    if (dynexpr->isAConstant())
        convertToConst(NULL);
}

int cStringParImpl::compare(const cParImpl *other) const
{
    int ret = cParImpl::compare(other);
    if (ret!=0)
        return ret;

    const cStringParImpl *other2 = dynamic_cast<const cStringParImpl *>(other);
    if (flags & FL_ISEXPR)
        return expr->compare(other2->expr);
    else
        return (val == other2->val) ? 0 : (val < other2->val) ? -1 : 1;
}

NAMESPACE_END

