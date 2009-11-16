/*****************************************************************
 *
 * This file is part of the GMAPPING project
 *
 * GMAPPING Copyright (c) 2004 Giorgio Grisetti, 
 * Cyrill Stachniss, and Wolfram Burgard
 *
 * This software is licensed under the "Creative Commons 
 * License (Attribution-NonCommercial-ShareAlike 2.0)" 
 * and is copyrighted by Giorgio Grisetti, Cyrill Stachniss, 
 * and Wolfram Burgard.
 * 
 * Further information on this license can be found at:
 * http://creativecommons.org/licenses/by-nc-sa/2.0/
 * 
 * GMAPPING is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied 
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  
 *
 *****************************************************************/


#ifndef MACRO_PARAMS_H
#define MACRO_PARAMS_H

#define PARAM_SET_GET(type, name, qualifier, setqualifier, getqualifier)\
qualifier: type m_##name;\
getqualifier: inline type get##name() const {return m_##name;}\
setqualifier: inline void set##name(type name) {m_##name=name;}

#define PARAM_SET(type, name, qualifier, setqualifier)\
qualifier: type m_##name;\
setqualifier: inline void set##name(type name) {m_##name=name;}

#define PARAM_GET(type, name, qualifier, getqualifier)\
qualifier: type m_##name;\
getqualifier: inline type get##name() const {return m_##name;}

#define MEMBER_PARAM_SET_GET(member, type, name, qualifier, setqualifier, getqualifier)\
getqualifier: inline type get##name() const {return member.get##name();}\
setqualifier: inline void set##name(type name) { member.set##name(name);}

#define MEMBER_PARAM_SET(member, type, name, qualifier, setqualifier, getqualifier)\
setqualifier: inline void set##name(type name) { member.set##name(name);}

#define MEMBER_PARAM_GET(member, type, name, qualifier, setqualifier, getqualifier)\
getqualifier: inline type get##name() const {return member.get##name();}

#define STRUCT_PARAM_SET_GET(member, type, name, qualifier, setqualifier, getqualifier)\
getqualifier: inline type get##name() const {return member.name;}\
setqualifier: inline void set##name(type name) {member.name=name;}

#define STRUCT_PARAM_SET(member, type, name, qualifier, setqualifier, getqualifier)\
setqualifier: inline void set##name(type name) {member.name=name;}

#define STRUCT_PARAM_GET(member, type, name, qualifier, setqualifier, getqualifier)\
getqualifier: inline type get##name() const {return member.name;}\

#define convertStringArgument(var,val,buf) if (!strcmp(buf,#val)) var=val
#endif
